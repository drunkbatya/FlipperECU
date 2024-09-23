#include "flipper_ecu_sync_worker_i.h"
#include "flipper_ecu_sync_worker_event_queue.h"

#include <furi_hal_resources.h>
#include <furi_hal_bus.h>
#include <furi_hal_power.h>
#include <furi_hal_interrupt.h>
#include <stm32wbxx_ll_tim.h>

#define TAG "FlipperECUSyncWorker"

#define GPIO_IGNITION_PIN_1 &gpio_ext_pe4
#define GPIO_IGNITION_PIN_2 &gpio_ext_pb4
//#define GPIO_IGNITION_PIN_3 &gpio_ext_pc3
//#define GPIO_IGNITION_PIN_4 &gpio_ext_pc4

#define GPIO_TIMER TIM2
#define GPIO_TIMER_BUS FuriHalBusTIM2
#define GPIO_TIMER_ISR_ID FuriHalInterruptIdTIM2

#define CKPS_TIMER TIM17
#define CKPS_TIMER_BUS FuriHalBusTIM17
#define CKPS_TIMER_ISR_ID FuriHalInterruptIdTim1TrgComTim17
#define CKPS_TIMER_GPIO_PIN &gpio_ext_pa7
#define CKPS_TIMER_GPIO_PIN_FN GpioAltFn14TIM17

#define CKPS_MISSED_TOOTH 2
#define CKPS_TOTAL_TOOTH_COUNT 60
#define CKPS_DEGREES_PER_INTERVAL (360 / CKPS_TOTAL_TOOTH_COUNT)
#define FIRST_CYLINDER_TDC_TOOTH_FROM_ZERO 20
#define SECOND_CYLINDER_TDC_TOOTH_FROM_ZERO 52

static void flipper_ecu_sync_worker_gpio_timer_deinit(FlipperECUSyncWorker* worker);
static void flipper_ecu_sync_worker_gpio_timer_init(FlipperECUSyncWorker* worker);
static void flipper_ecu_sync_worker_ckps_timer_deinit(FlipperECUSyncWorker* worker);
static void flipper_ecu_sync_worker_ckps_timer_init(FlipperECUSyncWorker* worker);

static inline uint32_t ms_to_ticks(uint32_t ms) {
    return ((SystemCoreClock / 1000) * ms);
}

static uint32_t degrees_to_ticks(uint32_t period_per_tooth, uint8_t degreese) {
    return (period_per_tooth / CKPS_DEGREES_PER_INTERVAL) * degreese;
}

// TODO: variable cylynder count and mode
static inline void flipper_ecu_sync_worker_make_predictions(FlipperECUSyncWorker* worker) {
    // we r captured sync period if we r here, to get actual period we need to divide sync period by missed teeth count
    uint32_t period_per_tooth = worker->previous_period;
    if(!worker->engine_status->synced) { // if engine is running again after stop
        furi_thread_flags_set(
            furi_thread_get_id(worker->thread), FlipperECUSyncWorkerEventEngineRunningAgain);
    }
    worker->engine_status->synced = true;
    worker->engine_status->rpm = SystemCoreClock / period_per_tooth;
    worker->engine_status->ign_angle = flipper_ecu_map_interpolate_2d(
        worker->engine_settings->maps[IGN_MAP], worker->engine_status->rpm);

    // semi-sequental ignition temp
    uint32_t timer_ticks_to_tdc_cylinder_1_4 =
        (period_per_tooth * FIRST_CYLINDER_TDC_TOOTH_FROM_ZERO);
    uint32_t ign_delay_cylinder_1_4 =
        timer_ticks_to_tdc_cylinder_1_4 -
        degrees_to_ticks(period_per_tooth, worker->engine_status->ign_angle);
    uint32_t timer_ticks_to_tdc_cylinder_2_3 =
        (period_per_tooth * SECOND_CYLINDER_TDC_TOOTH_FROM_ZERO);
    uint32_t ign_delay_cylinder_2_3 =
        timer_ticks_to_tdc_cylinder_2_3 -
        degrees_to_ticks(period_per_tooth, worker->engine_status->ign_angle);
    uint32_t dwell = ms_to_ticks(3);

    GPIO_QUEUE_ADD(worker, 1, ign_delay_cylinder_1_4 - dwell, GPIO_IGNITION_PIN_1, false);
    GPIO_QUEUE_ADD(worker, 1, ign_delay_cylinder_1_4, GPIO_IGNITION_PIN_1, true);

    GPIO_QUEUE_ADD(worker, 1, ign_delay_cylinder_2_3 - dwell, GPIO_IGNITION_PIN_2, false);
    GPIO_QUEUE_ADD(worker, 1, ign_delay_cylinder_2_3, GPIO_IGNITION_PIN_2, true);

    // semi-sequental injection temp
    //GPIO_QUEUE_ADD(worker, 2, ign_delay_cylinder_1_4 - dwell, GPIO_IGNITION_PIN_1, false);
    //GPIO_QUEUE_ADD(worker, 2, ign_delay_cylinder_1_4, GPIO_IGNITION_PIN_1, true);

    //GPIO_QUEUE_ADD(worker, 2, ign_delay_cylinder_2_3 - dwell, GPIO_IGNITION_PIN_2, false);
    //GPIO_QUEUE_ADD(worker, 2, ign_delay_cylinder_2_3, GPIO_IGNITION_PIN_2, true);

}

static inline void
    flipper_ecu_sync_worker_process_ckps_tick(FlipperECUSyncWorker* worker, uint32_t timer_value) {
    worker->previous_period = worker->current_period;
    worker->current_period = timer_value;
    // if current period equals missed teeth count, but not much more
    if(worker->previous_period != 0 && worker->current_period != 0) {
        if((worker->current_period >= (worker->previous_period * 2.5)) &&
           (worker->current_period < (worker->previous_period * 3.5))) {
            FURI_CRITICAL_ENTER();
            //if(!GPIO_QUEUE_IS_FULLY_EMPTY(worker)) {
            //    furi_crash("Timer queue isn't empty!");
            //}
            GPIO_QUEUE_RESET(worker);
            LL_TIM_GenerateEvent_UPDATE(GPIO_TIMER);
            //LL_TIM_DisableIT_CC1(CKPS_TIMER);
            //LL_TIM_DisableCounter(CKPS_TIMER);

            flipper_ecu_sync_worker_make_predictions(worker);

            //worker->current_period = 0;
            //worker->previous_period = 0;
            FURI_CRITICAL_EXIT();
            //LL_TIM_EnableCounter(CKPS_TIMER);
            //LL_TIM_EnableIT_CC1(CKPS_TIMER);
        } else { // basic tick
            worker->engine_status->rpm = SystemCoreClock / worker->current_period;
            worker->engine_status->ign_angle = flipper_ecu_map_interpolate_2d(
                worker->engine_settings->maps[IGN_MAP], worker->engine_status->rpm);
        }
    }
}

static void flipper_ecu_sync_worker_cpks_timer_isr(void* context) {
    FlipperECUSyncWorker* worker = context;
    if(LL_TIM_IsActiveFlag_CC1(CKPS_TIMER)) {
        LL_TIM_ClearFlag_CC1(CKPS_TIMER);
        LL_TIM_SetCounter(CKPS_TIMER, 0);
        uint32_t timer_value =
            LL_TIM_IC_GetCaptureCH1(CKPS_TIMER) + (worker->ckps_timer_overflows * 0xFFFF);
        worker->ckps_timer_overflows = 0;
        flipper_ecu_sync_worker_process_ckps_tick(worker, timer_value);
    } else if(LL_TIM_IsActiveFlag_UPDATE(CKPS_TIMER)) { // period elapsed
        LL_TIM_ClearFlag_UPDATE(CKPS_TIMER);
        worker->ckps_timer_overflows += 1;
        if(worker->ckps_timer_overflows > (SystemCoreClock / 10 / 0xFFFF)) { // 100ms - 1/10s
            FURI_CRITICAL_ENTER();
            worker->ckps_timer_overflows = 0;
            worker->engine_status->synced = false;
            worker->engine_status->ign_angle = 0;
            worker->engine_status->rpm = 0;
            worker->current_period = 0;
            worker->previous_period = 0;
            FURI_CRITICAL_EXIT();
            furi_thread_flags_set(
                furi_thread_get_id(worker->thread), FlipperECUSyncWorkerEventEngineStopped);
        }
    }
}

static void flipper_ecu_sync_worker_gpio_timer_isr(void* context) {
    FlipperECUSyncWorker* worker = context;
    if(LL_TIM_IsActiveFlag_CC1(GPIO_TIMER)) {
        LL_TIM_ClearFlag_CC1(GPIO_TIMER);
        FURI_CRITICAL_ENTER();
        if(!GPIO_QUEUE_IS_EMPTY(worker, 1)) {
            GPIOTimerEvent* event = GPIO_QUEUE_GET(worker, 1);
            furi_hal_gpio_write(event->gpio_pin, event->pin_state);
            if(event->next_compare_value != 0) {
                LL_TIM_OC_SetCompareCH1(GPIO_TIMER, event->next_compare_value);
            }
        }
        FURI_CRITICAL_EXIT();
    }
    if(LL_TIM_IsActiveFlag_CC2(GPIO_TIMER)) {
        LL_TIM_ClearFlag_CC2(GPIO_TIMER);
        FURI_CRITICAL_ENTER();
        if(!GPIO_QUEUE_IS_EMPTY(worker, 2)) {
            GPIOTimerEvent* event = GPIO_QUEUE_GET(worker, 2);
            furi_hal_gpio_write(event->gpio_pin, event->pin_state);
            if(event->next_compare_value != 0) {
                LL_TIM_OC_SetCompareCH2(GPIO_TIMER, event->next_compare_value);
            }
        }
        FURI_CRITICAL_EXIT();
    }
    if(LL_TIM_IsActiveFlag_CC3(GPIO_TIMER)) {
        LL_TIM_ClearFlag_CC3(GPIO_TIMER);
        FURI_CRITICAL_ENTER();
        if(!GPIO_QUEUE_IS_EMPTY(worker, 3)) {
            GPIOTimerEvent* event = GPIO_QUEUE_GET(worker, 3);
            furi_hal_gpio_write(event->gpio_pin, event->pin_state);
            if(event->next_compare_value != 0) {
                LL_TIM_OC_SetCompareCH3(GPIO_TIMER, event->next_compare_value);
            }
        }
        FURI_CRITICAL_EXIT();
    }
    if(LL_TIM_IsActiveFlag_CC4(GPIO_TIMER)) {
        LL_TIM_ClearFlag_CC4(GPIO_TIMER);
        FURI_CRITICAL_ENTER();
        if(!GPIO_QUEUE_IS_EMPTY(worker, 4)) {
            GPIOTimerEvent* event = GPIO_QUEUE_GET(worker, 4);
            furi_hal_gpio_write(event->gpio_pin, event->pin_state);
            if(event->next_compare_value != 0) {
                LL_TIM_OC_SetCompareCH4(GPIO_TIMER, event->next_compare_value);
            }
        }
        FURI_CRITICAL_EXIT();
    }
    if(LL_TIM_IsActiveFlag_UPDATE(GPIO_TIMER)) {
        LL_TIM_ClearFlag_UPDATE(GPIO_TIMER);
    }
}

static void flipper_ecu_sync_worker_gpio_default(FlipperECUSyncWorker* worker) {
    UNUSED(worker);
    furi_hal_gpio_write(GPIO_IGNITION_PIN_1, 0);
    furi_hal_gpio_write(GPIO_IGNITION_PIN_2, 0);
}

static int32_t flipper_ecu_sync_worker_thread(void* arg) {
    FlipperECUSyncWorker* worker = arg;
    uint32_t events;
    FURI_LOG_I(TAG, "thread started");
    furi_hal_power_insomnia_enter();
    while(1) {
        events =
            furi_thread_flags_wait(FlipperECUSyncWorkerEventAll, FuriFlagWaitAny, FuriWaitForever);
        if(events & FlipperECUSyncWorkerEventStop) {
            break;
        }
        if(events & FlipperECUSyncWorkerEventEngineStopped) {
            flipper_ecu_sync_worker_gpio_default(worker);
            flipper_ecu_fuel_pump_notify_ignition_engine_stopped(
                flipper_ecu_app_get_fuel_pump_worker(worker->ecu_app));
        }
        if(events & FlipperECUSyncWorkerEventEngineRunningAgain) {
            flipper_ecu_fuel_pump_notify_ignition_engine_running(
                flipper_ecu_app_get_fuel_pump_worker(worker->ecu_app));
        }
        if(events & FlipperECUSyncWorkerEventIgnitionSwitchedOn) {
            flipper_ecu_fuel_pump_notify_ignition_switched_on(
                flipper_ecu_app_get_fuel_pump_worker(worker->ecu_app));
            flipper_ecu_idle_valve_worker_notify_ignition_switched_on(
                flipper_ecu_app_get_idle_valve_worker(worker->ecu_app));
        }
        furi_delay_tick(10);
    }
    FURI_LOG_I(TAG, "thread stopped");
    furi_hal_power_insomnia_exit();
    return 0;
}

void flipper_ecu_sync_worker_send_stop(FlipperECUSyncWorker* worker) {
    flipper_ecu_sync_worker_gpio_timer_deinit(worker);
    flipper_ecu_sync_worker_ckps_timer_deinit(worker);

    furi_thread_flags_set(furi_thread_get_id(worker->thread), FlipperECUSyncWorkerEventStop);
}

static void flipper_ecu_sync_worker_ckps_timer_deinit(FlipperECUSyncWorker* worker) {
    UNUSED(worker);
    LL_TIM_DisableCounter(CKPS_TIMER);
    furi_hal_interrupt_set_isr(CKPS_TIMER_ISR_ID, NULL, NULL);
    furi_hal_gpio_init_ex(
        CKPS_TIMER_GPIO_PIN, GpioModeAnalog, GpioPullNo, GpioSpeedLow, GpioAltFnUnused);
    LL_TIM_DisableIT_CC1(CKPS_TIMER);
    LL_TIM_DisableIT_UPDATE(CKPS_TIMER);
    furi_hal_bus_disable(CKPS_TIMER_BUS);
}

void flipper_ecu_sync_worker_await_stop(FlipperECUSyncWorker* worker) {
    furi_thread_join(worker->thread);
}

static void flipper_ecu_sync_worker_ckps_timer_init(FlipperECUSyncWorker* worker) {
    furi_hal_bus_enable(CKPS_TIMER_BUS);

    furi_hal_gpio_init_ex(
        CKPS_TIMER_GPIO_PIN,
        GpioModeAltFunctionPushPull,
        GpioPullNo,
        GpioSpeedVeryHigh,
        CKPS_TIMER_GPIO_PIN_FN);

    // Timer: base
    LL_TIM_InitTypeDef TIM_InitStruct = {0};
    //TIM_InitStruct.Autoreload = (SystemCoreClock / 10); // 100ms
    TIM_InitStruct.Autoreload = 0xFFFF;
    LL_TIM_Init(CKPS_TIMER, &TIM_InitStruct);

    LL_TIM_IC_SetActiveInput(CKPS_TIMER, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_DIRECTTI);
    LL_TIM_IC_SetPrescaler(CKPS_TIMER, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);
    if(worker->engine_config.ckps_polarity == CKPSPolatityFalling) {
        LL_TIM_IC_SetPolarity(CKPS_TIMER, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_FALLING);
    } else {
        LL_TIM_IC_SetPolarity(CKPS_TIMER, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_RISING);
    }
    LL_TIM_IC_SetFilter(CKPS_TIMER, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV1);
    //LL_TIM_SetOnePulseMode(CKPS_TIMER, LL_TIM_ONEPULSEMODE_SINGLE);

    LL_TIM_EnableIT_CC1(CKPS_TIMER);
    LL_TIM_EnableIT_UPDATE(CKPS_TIMER);
    LL_TIM_CC_EnableChannel(CKPS_TIMER, LL_TIM_CHANNEL_CH1);
    LL_TIM_SetCounter(CKPS_TIMER, 0);

    furi_hal_interrupt_set_isr(CKPS_TIMER_ISR_ID, flipper_ecu_sync_worker_cpks_timer_isr, worker);

    LL_TIM_EnableCounter(CKPS_TIMER);
}

static void flipper_ecu_sync_worker_gpio_timer_deinit(FlipperECUSyncWorker* worker) {
    UNUSED(worker);
    LL_TIM_DisableCounter(GPIO_TIMER);
    LL_TIM_DisableIT_CC1(GPIO_TIMER);
    LL_TIM_DisableIT_CC2(GPIO_TIMER);
    LL_TIM_DisableIT_CC3(GPIO_TIMER);
    LL_TIM_DisableIT_CC4(GPIO_TIMER);
    LL_TIM_DisableIT_UPDATE(GPIO_TIMER);
    furi_hal_interrupt_set_isr(GPIO_TIMER_ISR_ID, NULL, NULL);
    furi_hal_bus_disable(GPIO_TIMER_BUS);
    furi_hal_gpio_init_ex(
        GPIO_IGNITION_PIN_1, GpioModeAnalog, GpioPullNo, GpioSpeedLow, GpioAltFnUnused);
    furi_hal_gpio_init_ex(
        GPIO_IGNITION_PIN_2, GpioModeAnalog, GpioPullNo, GpioSpeedLow, GpioAltFnUnused);
    //furi_hal_gpio_init_ex(
    //    GPIO_IGNITION_PIN_3, GpioModeAnalog, GpioPullNo, GpioSpeedLow, GpioAltFnUnused);
    //furi_hal_gpio_init_ex(
    //    GPIO_IGNITION_PIN_4, GpioModeAnalog, GpioPullNo, GpioSpeedLow, GpioAltFnUnused);
}

static void flipper_ecu_sync_worker_gpio_timer_init(FlipperECUSyncWorker* worker) {
    UNUSED(worker);
    //furi_hal_bus_enable(GPIO_TIMER_BUS);

    furi_hal_gpio_init_ex(
        GPIO_IGNITION_PIN_1,
        GpioModeOutputPushPull,
        GpioPullDown,
        GpioSpeedVeryHigh,
        GpioAltFnUnused);

    furi_hal_gpio_init_ex(
        GPIO_IGNITION_PIN_2,
        GpioModeOutputPushPull,
        GpioPullDown,
        GpioSpeedVeryHigh,
        GpioAltFnUnused);

    //furi_hal_gpio_init_ex(
    //    GPIO_IGNITION_PIN_3,
    //    GpioModeOutputPushPull,
    //    GpioPullDown,
    //    GpioSpeedVeryHigh,
    //    GpioAltFnUnused);

    //furi_hal_gpio_init_ex(
    //    GPIO_IGNITION_PIN_4,
    //    GpioModeOutputPushPull,
    //    GpioPullDown,
    //    GpioSpeedVeryHigh,
    //    GpioAltFnUnused);

    //LL_TIM_EnableIT_CC1(GPIO_TIMER);
    //LL_TIM_EnableIT_UPDATE(GPIO_TIMER);
    //LL_TIM_CC_EnableChannel(GPIO_TIMER, LL_TIM_CHANNEL_CH1);
    //LL_TIM_SetCounter(GPIO_TIMER, 1);

    //furi_hal_interrupt_set_isr(GPIO_TIMER_ISR_ID, flipper_ecu_sync_worker_gpio_timer_isr, worker);

    //LL_TIM_EnableCounter(GPIO_TIMER);
    //LL_TIM_GenerateEvent_UPDATE(GPIO_TIMER);
    //LL_TIM_EnableAllOutputs(GPIO_TIMER);
    //LL_TIM_GenerateEvent_CC1(GPIO_TIMER);
    //
    //
    furi_hal_bus_enable(GPIO_TIMER_BUS);

    furi_hal_interrupt_set_isr(GPIO_TIMER_ISR_ID, flipper_ecu_sync_worker_gpio_timer_isr, worker);

    LL_TIM_SetPrescaler(GPIO_TIMER, 0);
    LL_TIM_SetCounterMode(GPIO_TIMER, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetAutoReload(GPIO_TIMER, UINT32_MAX - 1); // max saved for empty next compare events
    LL_TIM_DisableARRPreload(GPIO_TIMER);
    LL_TIM_SetRepetitionCounter(GPIO_TIMER, 0);
    LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_RESET);
    LL_TIM_DisableMasterSlaveMode(TIM2);
    LL_TIM_SetClockDivision(GPIO_TIMER, LL_TIM_CLOCKDIVISION_DIV1);
    LL_TIM_SetClockSource(GPIO_TIMER, LL_TIM_CLOCKSOURCE_INTERNAL);

    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
    LL_TIM_DisableARRPreload(TIM2);
    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_FROZEN;
    TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.CompareValue = 0;
    TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
    LL_TIM_OC_Init(TIM2, LL_TIM_CHANNEL_CH1, &TIM_OC_InitStruct);
    LL_TIM_OC_DisableFast(TIM2, LL_TIM_CHANNEL_CH1);

    //LL_TIM_OC_SetCompareCH1(GPIO_TIMER, ms_to_ticks(1000));
    LL_TIM_EnableIT_CC1(GPIO_TIMER);
    LL_TIM_GenerateEvent_UPDATE(GPIO_TIMER);
    //LL_TIM_EnableIT_UPDATE(GPIO_TIMER);

    LL_TIM_EnableCounter(GPIO_TIMER);
    flipper_ecu_sync_worker_gpio_default(worker);
}

void flipper_ecu_sync_worker_start(FlipperECUSyncWorker* worker) {
    GPIO_QUEUE_RESET(worker);
    furi_thread_start(worker->thread);
    furi_delay_tick(10);
    flipper_ecu_sync_worker_ckps_timer_init(worker);
    flipper_ecu_sync_worker_gpio_timer_init(worker);
}

void flipper_ecu_sync_worker_load_engine_config(FlipperECUSyncWorker* worker) {
    worker->engine_config.ckps_polarity = CKPSPolatityFalling;
}

FlipperECUSyncWorker* flipper_ecu_sync_worker_alloc(
    FlipperECUApp* ecu_app,
    FlipperECUEngineStatus* engine_status,
    FlipperECUEngineSettings* engine_settings) {
    FlipperECUSyncWorker* worker = malloc(sizeof(FlipperECUSyncWorker));

    worker->thread = furi_thread_alloc_ex(TAG, 4092, flipper_ecu_sync_worker_thread, worker);
    worker->current_period = 0;
    worker->previous_period = 0;
    worker->ecu_app = ecu_app;
    worker->engine_status = engine_status;
    worker->engine_settings = engine_settings;
    flipper_ecu_sync_worker_load_engine_config(worker);
    worker->engine_status->synced = false;
    worker->engine_status->rpm = 0;
    worker->engine_status->ign_angle = 0;

    furi_delay_tick(2);

    return worker;
}

const FlipperECUEngineConfig* flipper_ecu_sync_worker_get_config(FlipperECUSyncWorker* worker) {
    return &(worker->engine_config);
}

void flipper_ecu_sync_worker_update_config_restart(
    FlipperECUSyncWorker* worker,
    const FlipperECUEngineConfig* config) {
    flipper_ecu_sync_worker_send_stop(worker);
    flipper_ecu_sync_worker_await_stop(worker);
    if(worker->engine_config.ckps_polarity != config->ckps_polarity) {
        worker->engine_config.ckps_polarity = config->ckps_polarity;
    }
    flipper_ecu_sync_worker_start(worker);
}

void flipper_ecu_sync_worker_load_config(
    FlipperECUSyncWorker* worker,
    const FlipperECUEngineConfig* config) {
    memcpy(&worker->engine_config, config, sizeof(FlipperECUEngineConfig));
}

void flipper_ecu_sync_worker_free(FlipperECUSyncWorker* worker) {
    furi_thread_free(worker->thread);
    free(worker);
}

void flipper_ecu_sync_worker_notify_ignition_switched_on(FlipperECUSyncWorker* worker) {
    furi_thread_flags_set(
        furi_thread_get_id(worker->thread), FlipperECUSyncWorkerEventIgnitionSwitchedOn);
}
