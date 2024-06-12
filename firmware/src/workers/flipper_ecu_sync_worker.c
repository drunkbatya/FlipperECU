#include "flipper_ecu_sync_worker.h"

#include <furi_hal_resources.h>
#include <furi_hal_bus.h>
#include <furi_hal_interrupt.h>
#include <stm32wbxx_ll_tim.h>

// debug
#include <furi_hal_pwm.h>

#define TAG "FlipperECUSyncWorker"

#define CKPS_TIMER TIM17
#define CKPS_TIMER_BUS FuriHalBusTIM17
#define CKPS_TIMER_ISR_ID FuriHalInterruptIdTim1TrgComTim17
#define CKPS_TIMER_GPIO_PIN &gpio_ext_pa7
#define CKPS_TIMER_GPIO_PIN_FN GpioAltFn14TIM17

#define CKPS_MISSED_TOOTH 2
#define CKPS_TOTAL_TOOTH_COUNT 60
#define CKPS_DEGREES_PER_INTERVAL (360 / CKPS_TOTAL_TOOTH_COUNT)
#define FIRST_CYLINDER_TDC_TOOTH_FROM_ZERO 19

static void flipper_ecu_sync_worker_ignition_timer_deinit(FlipperECUSyncWorker* worker);
static void flipper_ecu_sync_worker_ignition_timer_init(FlipperECUSyncWorker* worker);
static void flipper_ecu_sync_worker_ckps_timer_deinit(FlipperECUSyncWorker* worker);
static void flipper_ecu_sync_worker_ckps_timer_init(FlipperECUSyncWorker* worker);

static uint32_t degrees_to_ticks(uint32_t degreese, uint32_t period_per_tooth) {
    return (period_per_tooth / CKPS_DEGREES_PER_INTERVAL) * degreese;
}

static uint32_t temp_function_to_get_degreese(FlipperECUSyncWorker* worker) {
    UNUSED(worker);
    return 10;
}

// TODO: variable cylynder count and mode
static inline void flipper_ecu_sync_worker_make_predictions(FlipperECUSyncWorker* worker) {
    // we r captured sync period if we r here, to get actual period we need to divide sync period by missed teeth count
    uint32_t period_per_tooth = (worker->current_period / CKPS_MISSED_TOOTH);
    uint32_t timer_ticks_to_tdc_cylinder_1_4 =
        (period_per_tooth * FIRST_CYLINDER_TDC_TOOTH_FROM_ZERO);
    uint32_t ign_delay_cylinder_1_4 =
        timer_ticks_to_tdc_cylinder_1_4 -
        degrees_to_ticks(period_per_tooth, temp_function_to_get_degreese(worker));
    worker->temp = ign_delay_cylinder_1_4;
    furi_thread_flags_set(
        furi_thread_get_id(worker->thread), FlipperECUSyncWorkerEventPredictionDone);
}

static inline void
    flipper_ecu_sync_worker_process_ckps_tick(FlipperECUSyncWorker* worker, uint32_t timer_value) {
    worker->previous_period = worker->current_period;
    worker->current_period = timer_value;
    // if current period equals missed teeth count, but not much more
    if(worker->previous_period != 0 && worker->current_period != 0) {
        if((worker->current_period >= (worker->previous_period * CKPS_MISSED_TOOTH)) &&
           (worker->current_period < (worker->previous_period * CKPS_MISSED_TOOTH * 2))) {
            LL_TIM_DisableIT_CC1(CKPS_TIMER);
            LL_TIM_DisableCounter(CKPS_TIMER);

            worker->synced = true;
            flipper_ecu_sync_worker_make_predictions(worker);

            worker->current_period = 0;
            worker->previous_period = 0;
            LL_TIM_EnableCounter(CKPS_TIMER);
            LL_TIM_EnableIT_CC1(CKPS_TIMER);
        }
    }
}

static inline void flipper_ecu_sync_worker_process_out_of_sync(FlipperECUSyncWorker* worker) {
    worker->synced = false;
    worker->current_period = 0;
    worker->previous_period = 0;
    furi_thread_flags_set(furi_thread_get_id(worker->thread), FlipperECUSyncWorkerEventCkpPulse);
}

static void flipper_ecu_sync_worker_timer2_isr(void* context) {
    FlipperECUSyncWorker* worker = context;
    if(LL_TIM_IsActiveFlag_CC1(CKPS_TIMER)) {
        LL_TIM_ClearFlag_CC1(CKPS_TIMER);
        LL_TIM_SetCounter(CKPS_TIMER, 0);
        uint32_t timer_value = LL_TIM_IC_GetCaptureCH1(CKPS_TIMER) + (worker->timer_overflows * 0xFFFF);
        worker->timer_overflows = 0;
        flipper_ecu_sync_worker_process_ckps_tick(worker, timer_value);
    } else if(LL_TIM_IsActiveFlag_UPDATE(CKPS_TIMER)) { // period elapsed
        LL_TIM_ClearFlag_UPDATE(CKPS_TIMER);
        worker->timer_overflows += 1;
        if(worker->timer_overflows > (SystemCoreClock / 10 / 0xFFFF)) { // 100ms - 1/10s
            worker->timer_overflows = 0;
            flipper_ecu_sync_worker_process_out_of_sync(worker);
        }
    }
}

static int32_t flipper_ecu_sync_worker_thread(void* arg) {
    FlipperECUSyncWorker* worker = arg;
    UNUSED(worker);
    uint32_t events;
    FuriString* fstr = furi_string_alloc();
    FURI_LOG_I(TAG, "thread started");
    while(1) {
        events =
            furi_thread_flags_wait(FlipperECUSyncWorkerEventAll, FuriFlagWaitAny, FuriWaitForever);
        if(events & FlipperECUSyncWorkerEventStop) {
            break;
        }
        if(events & FlipperECUSyncWorkerEventCkpPulse) {
            FURI_LOG_I(TAG, "Timer elapsed!");
        }
        if(events & FlipperECUSyncWorkerEventPredictionDone) {
            furi_string_printf(fstr, "Prediction done! Value: %lu", worker->temp);
            FURI_LOG_I(TAG, furi_string_get_cstr(fstr));
        }
        furi_delay_tick(1);
    }
    furi_string_free(fstr);
    FURI_LOG_I(TAG, "thread stopped");
    return 0;
}

void flipper_ecu_sync_worker_send_stop(FlipperECUSyncWorker* worker) {
    flipper_ecu_sync_worker_ignition_timer_deinit(worker);
    flipper_ecu_sync_worker_ckps_timer_deinit(worker);
    if(furi_hal_pwm_is_running(FuriHalPwmOutputIdLptim2PA4)) {
        furi_hal_pwm_stop(FuriHalPwmOutputIdLptim2PA4);
    }

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

static void flipper_ecu_sync_worker_ckps_timer_init(FlipperECUSyncWorker* worker) {
    furi_hal_bus_enable(CKPS_TIMER_BUS);

    furi_hal_gpio_init_ex(
        CKPS_TIMER_GPIO_PIN, GpioModeAltFunctionPushPull, GpioPullNo, GpioSpeedVeryHigh, CKPS_TIMER_GPIO_PIN_FN);

    // Timer: base
    LL_TIM_InitTypeDef TIM_InitStruct = {0};
    //TIM_InitStruct.Autoreload = (SystemCoreClock / 10); // 100ms
    TIM_InitStruct.Autoreload = 0xFFFF;
    LL_TIM_Init(CKPS_TIMER, &TIM_InitStruct);

    LL_TIM_IC_SetActiveInput(CKPS_TIMER, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_DIRECTTI);
    LL_TIM_IC_SetPrescaler(CKPS_TIMER, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);
    LL_TIM_IC_SetPolarity(CKPS_TIMER, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_RISING);
    LL_TIM_IC_SetFilter(CKPS_TIMER, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV1);
    //LL_TIM_SetOnePulseMode(CKPS_TIMER, LL_TIM_ONEPULSEMODE_SINGLE);

    LL_TIM_EnableIT_CC1(CKPS_TIMER);
    LL_TIM_EnableIT_UPDATE(CKPS_TIMER);
    LL_TIM_CC_EnableChannel(CKPS_TIMER, LL_TIM_CHANNEL_CH1);
    LL_TIM_SetCounter(CKPS_TIMER, 0);

    furi_hal_interrupt_set_isr(CKPS_TIMER_ISR_ID, flipper_ecu_sync_worker_timer2_isr, worker);

    LL_TIM_EnableCounter(CKPS_TIMER);
}

static void flipper_ecu_sync_worker_ignition_timer_deinit(FlipperECUSyncWorker* worker) {
    UNUSED(worker);
}
static void flipper_ecu_sync_worker_ignition_timer_init(FlipperECUSyncWorker* worker) {
    UNUSED(worker);
}

FlipperECUSyncWorker*
    flipper_ecu_sync_worker_alloc(FlipperECUSettings* settings, const FlipperECUGPIO* gpio) {
    FlipperECUSyncWorker* worker = malloc(sizeof(FlipperECUSyncWorker));
    worker->thread = furi_thread_alloc_ex(TAG, 1024, flipper_ecu_sync_worker_thread, worker);
    worker->settings = settings;
    worker->gpio = gpio;
    worker->synced = false;
    worker->current_period = 0;
    worker->previous_period = 0;

    flipper_ecu_sync_worker_ckps_timer_init(worker);
    flipper_ecu_sync_worker_ignition_timer_init(worker);

    if(furi_hal_pwm_is_running(FuriHalPwmOutputIdLptim2PA4)) {
        furi_hal_pwm_stop(FuriHalPwmOutputIdLptim2PA4);
    }
    furi_hal_pwm_start(FuriHalPwmOutputIdLptim2PA4, 1100, 50);

    return worker;
}

void flipper_ecu_sync_worker_free(FlipperECUSyncWorker* worker) {
    furi_thread_free(worker->thread);
    free(worker);
}

void flipper_ecu_sync_worker_start(FlipperECUSyncWorker* worker) {
    furi_thread_start(worker->thread);
    furi_delay_tick(1);
}

void flipper_ecu_sync_worker_await_stop(FlipperECUSyncWorker* worker) {
    furi_thread_join(worker->thread);
}

uint32_t flipper_ecu_sync_worker_get_rpm(FlipperECUSyncWorker* worker) {
    return worker->current_period;
}
