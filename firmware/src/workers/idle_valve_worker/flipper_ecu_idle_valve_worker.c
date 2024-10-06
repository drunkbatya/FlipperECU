#include <furi_hal_gpio.h>
#include <furi_hal_bus.h>
#include <furi_hal_interrupt.h>
#include <stm32wbxx_ll_lptim.h>
#include <stm32wbxx_ll_tim.h>
#include <stm32wbxx_ll_rcc.h>

#include "flipper_ecu_idle_valve_worker_i.h"
#include "../../flipper_ecu_resources.h"

#define TAG "FlipperECUIdleValveWorker"

const uint32_t lptim_psc_table[] = {
    LL_LPTIM_PRESCALER_DIV1,
    LL_LPTIM_PRESCALER_DIV2,
    LL_LPTIM_PRESCALER_DIV4,
    LL_LPTIM_PRESCALER_DIV8,
    LL_LPTIM_PRESCALER_DIV16,
    LL_LPTIM_PRESCALER_DIV32,
    LL_LPTIM_PRESCALER_DIV64,
    LL_LPTIM_PRESCALER_DIV128,
};

static void flipper_ecu_idle_valve_worker_stop_timer_isr(void* context) {
    FlipperECUIdleValveWorker* worker = context;
    if(LL_TIM_IsActiveFlag_UPDATE(TIM1)) {
        LL_TIM_ClearFlag_UPDATE(TIM1);
        if(worker->stop_timer_current_overflow_count < worker->stop_timer_max_overflow_count) {
            if(worker->stop_timer_current_overflow_count ==
               worker->stop_timer_max_overflow_count - 2) {
                if(worker->stop_timer_last_reload) {
                    LL_TIM_SetAutoReload(TIM1, worker->stop_timer_last_reload);
                }
            }
            worker->stop_timer_current_overflow_count++;
        } else {
            if(worker->calibration_ongoing) {
                worker->calibration_ongoing = false;
                LL_TIM_SetAutoReload(TIM1, UINT16_MAX);
                worker->stop_timer_current_overflow_count = 0;
                furi_hal_gpio_write(
                    gpio_mcu_idle_direction, !furi_hal_gpio_read(gpio_mcu_idle_direction));
            } else {
                worker->stop_timer_current_overflow_count = 0;
                LL_TIM_DisableCounter(TIM1);
                LL_LPTIM_StartCounter(
                    LPTIM2, LL_LPTIM_OPERATING_MODE_ONESHOT); // disabling continuous mode
                worker->calibration_done = true;
                worker->movement_done = true; // TODO separate
            }
        }
    }
}

static int32_t flipper_ecu_idle_valve_worker_thread(void* arg) {
    FlipperECUIdleValveWorker* worker = arg;
    FuriString* fstr = furi_string_alloc();
    FlipperECUAdcWorker* adc_worker = flipper_ecu_app_get_adc_worker(worker->ecu_app);
    uint32_t events;
    FURI_LOG_I(TAG, "thread started");
    while(1) {
        events = furi_thread_flags_get();
        if(events & FlipperECUIdleValveWorkerEventStop) {
            break;
        }
        if(events & FlipperECUIdleValveWorkerEventIgnitionSwitchedOn) {
            flipper_ecu_idle_valve_worker_calibrate(worker);
            flipper_ecu_idle_valve_worker_move_to(
                worker, worker->engine_settings->idle_valve_position_on_ignition_on);
            furi_thread_flags_clear(FlipperECUIdleValveWorkerEventIgnitionSwitchedOn);
        }
        if((worker->engine_settings->idle_closed_loop == false) &&
           (worker->engine_status->mode == EngineModeIdle)) {
            const uint16_t allowed_diff_rpm = 40;
            double water_temp = flipper_ecu_adc_worker_get_value_water_temp_full(adc_worker);
            int16_t target_rpm = flipper_ecu_map_interpolate_2d(
                worker->engine_settings->maps[IDLE_RPM], water_temp);
            if(worker->engine_status->rpm < (target_rpm - allowed_diff_rpm)) {
                flipper_ecu_idle_valve_worker_step(worker, IdleValveDirectionForward);
            } else if(worker->engine_status->rpm > (target_rpm + allowed_diff_rpm)) {
                flipper_ecu_idle_valve_worker_step(worker, IdleValveDirectionBackward);
            }
        }
        furi_delay_tick(5);
    }
    furi_string_free(fstr);
    FURI_LOG_I(TAG, "thread stopped");
    return 0;
}

static void flipper_ecu_idle_valve_worker_gpio_step_init(FlipperECUIdleValveWorker* worker) {
    UNUSED(worker);
    furi_hal_gpio_init_ex(
        gpio_mcu_idle_step,
        GpioModeAltFunctionPushPull,
        GpioPullNo,
        GpioSpeedVeryHigh,
        GpioAltFn14LPTIM2);
}

static void flipper_ecu_idle_valve_worker_gpio_step_deinit(FlipperECUIdleValveWorker* worker) {
    UNUSED(worker);
    furi_hal_gpio_init_simple(gpio_mcu_idle_step, GpioModeAnalog);
}

static void flipper_ecu_idle_valve_worker_gpio_dir_init(FlipperECUIdleValveWorker* worker) {
    UNUSED(worker);
    furi_hal_gpio_init_ex(
        gpio_mcu_idle_direction,
        GpioModeOutputPushPull,
        GpioPullNo,
        GpioSpeedVeryHigh,
        GpioAltFnUnused);
}

static void flipper_ecu_idle_valve_worker_gpio_dir_deinit(FlipperECUIdleValveWorker* worker) {
    UNUSED(worker);
    furi_hal_gpio_init_simple(gpio_mcu_idle_direction, GpioModeAnalog);
}

static void flipper_ecu_idle_valve_worker_pwm_timer_set_freq(FlipperECUIdleValveWorker* worker) {
    uint32_t freq_div = 64000000LU / worker->engine_settings->idle_valve_pwm_freq;
    uint32_t prescaler = 0;
    uint32_t period = 0;

    bool clock_lse = false;

    do {
        period = freq_div / (1UL << prescaler);
        if(period <= 0xFFFF) {
            break;
        }
        prescaler++;
        if(prescaler > 7) {
            prescaler = 0;
            clock_lse = true;
            period = 32768LU / worker->engine_settings->idle_valve_pwm_freq;
            break;
        }
    } while(1);

    uint32_t compare = period * 50 / 100;

    LL_LPTIM_SetPrescaler(LPTIM2, lptim_psc_table[prescaler]);
    LL_LPTIM_SetAutoReload(LPTIM2, period);
    LL_LPTIM_SetCompare(LPTIM2, compare);

    if(clock_lse) {
        LL_RCC_SetLPTIMClockSource(LL_RCC_LPTIM2_CLKSOURCE_LSE);
    } else {
        LL_RCC_SetLPTIMClockSource(LL_RCC_LPTIM2_CLKSOURCE_PCLK1);
    }
}

static void flipper_ecu_idle_valve_worker_pwm_timer_init(FlipperECUIdleValveWorker* worker) {
    if(!furi_hal_bus_is_enabled(FuriHalBusLPTIM2)) furi_hal_bus_enable(FuriHalBusLPTIM2);
    LL_LPTIM_SetUpdateMode(LPTIM2, LL_LPTIM_UPDATE_MODE_ENDOFPERIOD);
    LL_RCC_SetLPTIMClockSource(LL_RCC_LPTIM2_CLKSOURCE_PCLK1);
    LL_LPTIM_SetClockSource(LPTIM2, LL_LPTIM_CLK_SOURCE_INTERNAL);
    LL_LPTIM_ConfigOutput(LPTIM2, LL_LPTIM_OUTPUT_WAVEFORM_PWM, LL_LPTIM_OUTPUT_POLARITY_INVERSE);
    LL_LPTIM_SetCounterMode(LPTIM2, LL_LPTIM_COUNTER_MODE_INTERNAL);

    LL_LPTIM_Enable(LPTIM2);

    flipper_ecu_idle_valve_worker_pwm_timer_set_freq(worker);
}

static void flipper_ecu_idle_valve_worker_pwm_timer_deinit(FlipperECUIdleValveWorker* worker) {
    UNUSED(worker);
    // LL_LPTIM_Disable(LPTIM2);  // Why it not in API?
    if(furi_hal_bus_is_enabled(FuriHalBusLPTIM2)) furi_hal_bus_disable(FuriHalBusLPTIM2);
}

static void flipper_ecu_idle_valve_worker_stop_timer_init(FlipperECUIdleValveWorker* worker) {
    if(!furi_hal_bus_is_enabled(FuriHalBusTIM1)) furi_hal_bus_enable(FuriHalBusTIM1);
    furi_hal_interrupt_set_isr(
        FuriHalInterruptIdTim1UpTim16, flipper_ecu_idle_valve_worker_stop_timer_isr, worker);
    LL_TIM_SetPrescaler(TIM1, 0);
    LL_TIM_SetCounterMode(TIM1, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetAutoReload(TIM1, UINT16_MAX);
    LL_TIM_DisableARRPreload(TIM1);
    LL_TIM_SetRepetitionCounter(TIM1, 0);
    LL_TIM_SetTriggerOutput(TIM1, LL_TIM_TRGO_RESET);
    LL_TIM_DisableMasterSlaveMode(TIM1);
    LL_TIM_SetClockDivision(TIM1, LL_TIM_CLOCKDIVISION_DIV1);
    LL_TIM_SetClockSource(TIM1, LL_TIM_CLOCKSOURCE_INTERNAL);

    LL_TIM_GenerateEvent_UPDATE(TIM1);
    LL_TIM_ClearFlag_UPDATE(TIM1);
    LL_TIM_EnableIT_UPDATE(TIM1);
}

static void flipper_ecu_idle_valve_worker_stop_timer_deinit(FlipperECUIdleValveWorker* worker) {
    UNUSED(worker);
    if(furi_hal_bus_is_enabled(FuriHalBusTIM1)) furi_hal_bus_disable(FuriHalBusTIM1);
    LL_TIM_DisableIT_CC2(TIM1);
    LL_TIM_DisableCounter(TIM1);
    furi_hal_interrupt_set_isr(FuriHalInterruptIdTim1UpTim16, NULL, NULL);
}

FlipperECUIdleValveWorker* flipper_ecu_idle_valve_worker_alloc(
    FlipperECUApp* ecu_app,
    FlipperECUEngineSettings* engine_settings,
    FlipperECUEngineStatus* engine_status) {
    FlipperECUIdleValveWorker* worker = malloc(sizeof(FlipperECUIdleValveWorker));
    worker->thread = furi_thread_alloc_ex(TAG, 1024, flipper_ecu_idle_valve_worker_thread, worker);
    worker->ecu_app = ecu_app;
    worker->engine_settings = engine_settings;
    worker->engine_status = engine_status;
    flipper_ecu_idle_valve_worker_gpio_dir_init(worker);
    flipper_ecu_idle_valve_worker_gpio_step_init(worker);
    flipper_ecu_idle_valve_worker_pwm_timer_init(worker);
    flipper_ecu_idle_valve_worker_stop_timer_init(worker);
    return worker;
}
void flipper_ecu_idle_valve_worker_start(FlipperECUIdleValveWorker* worker) {
    furi_thread_start(worker->thread);
    furi_delay_tick(10);
    //LL_TIM_EnableCounter(TIM1);
    //LL_TIM_OC_SetCompareCH1(TIM1, 20000);
}

void flipper_ecu_idle_valve_worker_free(FlipperECUIdleValveWorker* worker) {
    flipper_ecu_idle_valve_worker_gpio_dir_deinit(worker);
    flipper_ecu_idle_valve_worker_gpio_step_deinit(worker);
    flipper_ecu_idle_valve_worker_pwm_timer_deinit(worker);
    flipper_ecu_idle_valve_worker_stop_timer_deinit(worker);
    furi_thread_free(worker->thread);
    free(worker);
}

void flipper_ecu_idle_valve_worker_send_stop(FlipperECUIdleValveWorker* worker) {
    furi_thread_flags_set(furi_thread_get_id(worker->thread), FlipperECUIdleValveWorkerEventStop);
}

void flipper_ecu_idle_valve_worker_await_stop(FlipperECUIdleValveWorker* worker) {
    furi_thread_join(worker->thread);
}

uint16_t flipper_ecu_idle_valve_worker_get_current_position(FlipperECUIdleValveWorker* worker) {
    return worker->current_position;
}

void flipper_ecu_idle_valve_worker_step(
    FlipperECUIdleValveWorker* worker,
    IdleValveDirection direction) {
    if(direction == IdleValveDirectionForward) {
        if(worker->current_position < worker->engine_settings->idle_valve_total_steps) {
            worker->current_position++;
            furi_hal_gpio_write(gpio_mcu_idle_direction, 1);
        }
    } else if(direction == IdleValveDirectionBackward) {
        if(worker->current_position > 0) {
            worker->current_position--;
            furi_hal_gpio_write(gpio_mcu_idle_direction, 0);
        }
    }
    LL_LPTIM_StartCounter(LPTIM2, LL_LPTIM_OPERATING_MODE_ONESHOT);
}

void flipper_ecu_idle_valve_worker_calibrate(FlipperECUIdleValveWorker* worker) {
    worker->calibration_ongoing = true;
    worker->calibration_done = false;
    uint16_t steps = worker->engine_settings->idle_valve_total_steps;
    uint32_t timer_ticks = (64000000 / worker->engine_settings->idle_valve_pwm_freq) * steps;
    uint16_t stop_timer_overflows = timer_ticks / UINT16_MAX;
    worker->stop_timer_last_reload = timer_ticks % UINT16_MAX;
    if(worker->stop_timer_last_reload) stop_timer_overflows++;
    worker->stop_timer_max_overflow_count = stop_timer_overflows;
    LL_TIM_SetAutoReload(TIM1, UINT16_MAX);

    furi_hal_gpio_write(gpio_mcu_idle_direction, 1); // going forward
    worker->stop_timer_current_overflow_count = 0;
    LL_LPTIM_StartCounter(LPTIM2, LL_LPTIM_OPERATING_MODE_CONTINUOUS);
    LL_TIM_EnableCounter(TIM1);
    while(!worker->calibration_done) furi_delay_tick(1);
    worker->current_position = 0;
}

void flipper_ecu_idle_valve_worker_move_to(FlipperECUIdleValveWorker* worker, uint16_t position) {
    uint16_t steps = 0;
    worker->movement_done = false;
    if(position > worker->current_position) {
        steps = position - worker->current_position;
        furi_hal_gpio_write(gpio_mcu_idle_direction, 1); // going forward
    } else if(position < worker->current_position) {
        steps = position + worker->current_position;
        furi_hal_gpio_write(gpio_mcu_idle_direction, 0); // going backward
    } else if(position == worker->current_position) {
        return;
    }
    uint32_t timer_ticks = (64000000 / worker->engine_settings->idle_valve_pwm_freq) * steps;
    uint16_t stop_timer_overflows = timer_ticks / UINT16_MAX;
    worker->stop_timer_last_reload = timer_ticks % UINT16_MAX;
    if(worker->stop_timer_last_reload) stop_timer_overflows++;
    worker->stop_timer_max_overflow_count = stop_timer_overflows;
    LL_TIM_SetAutoReload(TIM1, UINT16_MAX);

    worker->stop_timer_current_overflow_count = 0;
    LL_LPTIM_StartCounter(LPTIM2, LL_LPTIM_OPERATING_MODE_CONTINUOUS);
    LL_TIM_EnableCounter(TIM1);
    while(!worker->movement_done) furi_delay_tick(1);
    worker->current_position = position;
}

void flipper_ecu_idle_valve_worker_notify_ignition_switched_on(FlipperECUIdleValveWorker* worker) {
    furi_thread_flags_set(
        furi_thread_get_id(worker->thread), FlipperECUIdleValveWorkerEventIgnitionSwitchedOn);
}
