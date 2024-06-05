#include "flipper_ecu_sync_worker.h"

#include <furi_hal_resources.h>
#include <furi_hal_bus.h>
#include <furi_hal_interrupt.h>
#include <stm32wbxx_ll_tim.h>

#define TAG "FlipperECUSyncWorker"

static uint32_t timer = 0;

static void flipper_ecu_sync_worker_process_ckp_tick(void* context) {
    FlipperECUSyncWorker* worker = context;
    if(LL_TIM_IsActiveFlag_CC1(TIM2)) {
        LL_TIM_ClearFlag_CC1(TIM2);
        timer = LL_TIM_IC_GetCaptureCH1(TIM2);
        LL_TIM_SetCounter(TIM2, 0);
        furi_thread_flags_set(
            furi_thread_get_id(worker->thread), FlipperECUSyncWorkerEventCkpPulse);
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
            FURI_LOG_I(TAG, "ckp_tick recived!");
            furi_string_printf(fstr, "RPS: %lu", (SystemCoreClock / timer));
            FURI_LOG_I(TAG, furi_string_get_cstr(fstr));
            continue;
        }
    }
    furi_string_free(fstr);
    FURI_LOG_I(TAG, "thread stopped");
    return 0;
}

void flipper_ecu_sync_worker_send_stop(FlipperECUSyncWorker* worker) {
    furi_thread_flags_set(furi_thread_get_id(worker->thread), FlipperECUSyncWorkerEventStop);
}

static void flipper_ecu_sync_worker_ckps_timer_init(FlipperECUSyncWorker* worker) {
    furi_hal_bus_enable(FuriHalBusTIM2);
    furi_hal_gpio_init_ex(
        &gpio_ext_pa15, GpioModeAltFunctionPushPull, GpioPullNo, GpioSpeedVeryHigh, GpioAltFn1TIM2);
    LL_TIM_IC_InitTypeDef init_struct = {0};
    init_struct.ICPolarity = LL_TIM_IC_POLARITY_RISING;
    init_struct.ICActiveInput = LL_TIM_ACTIVEINPUT_DIRECTTI;
    init_struct.ICPrescaler = LL_TIM_ICPSC_DIV1;
    init_struct.ICFilter = LL_TIM_IC_FILTER_FDIV1;
    LL_TIM_IC_Init(TIM2, LL_TIM_CHANNEL_CH1, &init_struct);
    LL_TIM_SetRepetitionCounter(TIM2, 0);
    LL_TIM_SetClockDivision(TIM2, LL_TIM_CLOCKDIVISION_DIV1);
    LL_TIM_SetClockSource(TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);
    LL_TIM_DisableARRPreload(TIM2);
    LL_TIM_EnableIT_CC1(TIM2);
    LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH1);

    LL_TIM_SetCounter(TIM2, 0);
    LL_TIM_EnableCounter(TIM2);

    furi_hal_interrupt_set_isr(
        FuriHalInterruptIdTIM2, flipper_ecu_sync_worker_process_ckp_tick, worker);
}

FlipperECUSyncWorker*
    flipper_ecu_sync_worker_alloc(FlipperECUSettings* settings, const FlipperECUGPIO* gpio) {
    FlipperECUSyncWorker* worker = malloc(sizeof(FlipperECUSyncWorker));
    worker->thread = furi_thread_alloc_ex(TAG, 1024, flipper_ecu_sync_worker_thread, worker);
    worker->settings = settings;
    worker->gpio = gpio;
    worker->current_tick = 0;
    worker->previous_tick = 0;
    worker->synced = false;

    flipper_ecu_sync_worker_ckps_timer_init(worker);

    return worker;
}

void flipper_ecu_sync_worker_free(FlipperECUSyncWorker* worker) {
    furi_hal_bus_disable(FuriHalBusTIM2);
    free(worker);
}

void flipper_ecu_sync_worker_start(FlipperECUSyncWorker* worker) {
    furi_thread_start(worker->thread);
    furi_delay_ms(10);
}

void flipper_ecu_sync_worker_await_stop(FlipperECUSyncWorker* worker) {
    furi_thread_join(worker->thread);
}

uint32_t flipper_ecu_sync_worker_get_rpm(FlipperECUSyncWorker* worker) {
    UNUSED(worker);
    return 12;
}
