#include "flipper_ecu_sync_worker.h"

#include <furi_hal_resources.h>
#include <furi_hal_bus.h>
#include <furi_hal_interrupt.h>
#include <stm32wbxx_ll_tim.h>

#define TAG "FlipperECUSyncWorker"

static void flipper_ecu_sync_worker_process_ckp_tick(void* context) {
    FlipperECUSyncWorker* worker = context;
    if(LL_TIM_IsActiveFlag_CC1(TIM2)) {
        uint32_t current_period = LL_TIM_IC_GetCaptureCH1(TIM2);
        LL_TIM_ClearFlag_CC1(TIM2);
        worker->previous_period = worker->current_period;
        worker->current_period = current_period;
        LL_TIM_SetCounter(TIM2, 0);
        furi_thread_flags_set(
            furi_thread_get_id(worker->thread), FlipperECUSyncWorkerEventCkpPulse);
    }
}

static int32_t flipper_ecu_sync_worker_thread(void* arg) {
    FlipperECUSyncWorker* worker = arg;
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
            FURI_LOG_I(TAG, "ckp_tick recived!");
            furi_string_printf(fstr, "RPS: %lu", (SystemCoreClock / worker->current_period));
            FURI_LOG_I(TAG, furi_string_get_cstr(fstr));
        }
        furi_delay_tick(1);
    }
    furi_string_free(fstr);
    FURI_LOG_I(TAG, "thread stopped");
    return 0;
}

void flipper_ecu_sync_worker_send_stop(FlipperECUSyncWorker* worker) {
    LL_TIM_DisableCounter(TIM2);
    furi_hal_interrupt_set_isr(FuriHalInterruptIdTIM2, NULL, NULL);

    furi_hal_gpio_init_ex(
        &gpio_ext_pa15, GpioModeAnalog, GpioPullNo, GpioSpeedLow, GpioAltFnUnused);
    furi_hal_bus_disable(FuriHalBusTIM2);
    furi_thread_flags_set(furi_thread_get_id(worker->thread), FlipperECUSyncWorkerEventStop);
}

static void flipper_ecu_sync_worker_ckps_timer_init(FlipperECUSyncWorker* worker) {
    furi_hal_bus_enable(FuriHalBusTIM2);

    furi_hal_gpio_init_ex(
        &gpio_ext_pa15, GpioModeAltFunctionPushPull, GpioPullNo, GpioSpeedVeryHigh, GpioAltFn1TIM2);

    // Timer: base
    LL_TIM_InitTypeDef TIM_InitStruct = {0};
    TIM_InitStruct.Prescaler = 0;
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload = 0x7FFFFFFE;
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    LL_TIM_Init(TIM2, &TIM_InitStruct);

    // Timer: advanced
    LL_TIM_SetClockSource(TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);
    LL_TIM_DisableARRPreload(TIM2);
    LL_TIM_SetRepetitionCounter(TIM2, 0);
    LL_TIM_SetClockDivision(TIM2, LL_TIM_CLOCKDIVISION_DIV1);
    LL_TIM_SetClockSource(TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);

    LL_TIM_IC_SetActiveInput(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_DIRECTTI);
    LL_TIM_IC_SetPrescaler(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);
    LL_TIM_IC_SetPolarity(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_RISING);
    LL_TIM_IC_SetFilter(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV1);

    LL_TIM_EnableIT_CC1(TIM2);
    LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH1);
    LL_TIM_SetCounter(TIM2, 0);

    furi_hal_interrupt_set_isr(
        FuriHalInterruptIdTIM2, flipper_ecu_sync_worker_process_ckp_tick, worker);

    LL_TIM_EnableCounter(TIM2);
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
    return SystemCoreClock / worker->current_period;
}
