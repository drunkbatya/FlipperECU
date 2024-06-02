#include "flipper_ecu_sync_worker.h"

#include <furi_hal_resources.h>

#define TAG "FlipperECUSyncWorker"

static int32_t flipper_ecu_sync_worker_thread(void* arg) {
    FlipperECUSyncWorker* worker = arg;
    UNUSED(worker);
    uint32_t events;
    FURI_LOG_I(TAG, "thread started");
    while(1) {
        events =
            furi_thread_flags_wait(FlipperECUSyncWorkerEventAll, FuriFlagWaitAny, FuriWaitForever);
        if(events & FlipperECUSyncWorkerEventStop) {
            break;
        }
        if(events & FlipperECUSyncWorkerEventCkpPulse) {
            FURI_LOG_I(TAG, "ckp_tick recived!");
            continue;
        }
    }
    FURI_LOG_I(TAG, "thread stopped");
    return 0;
}

void flipper_ecu_sync_worker_send_stop(FlipperECUSyncWorker* worker) {
    furi_hal_gpio_remove_int_callback(worker->gpio->ckp_pin);
    furi_thread_flags_set(furi_thread_get_id(worker->thread), FlipperECUSyncWorkerEventStop);
}

static void flipper_ecu_sync_worker_process_ckp_tick(void* context) {
    FlipperECUSyncWorker* worker = context;
    furi_thread_flags_set(furi_thread_get_id(worker->thread), FlipperECUSyncWorkerEventCkpPulse);
}

FlipperECUSyncWorker*
    flipper_ecu_sync_worker_alloc(FlipperECUSettings* settings, const FlipperECUGPIO* gpio) {
    FlipperECUSyncWorker* worker = malloc(sizeof(FlipperECUSyncWorker));
    worker->thread = furi_thread_alloc_ex(TAG, 1024, flipper_ecu_sync_worker_thread, worker);
    worker->settings = settings;
    worker->gpio = gpio;
    return worker;
}

void flipper_ecu_sync_worker_free(FlipperECUSyncWorker* worker) {
    free(worker);
}

void flipper_ecu_sync_worker_start(FlipperECUSyncWorker* worker) {
    furi_thread_start(worker->thread);
    furi_delay_ms(10);
    furi_hal_gpio_add_int_callback(
        worker->gpio->ckp_pin, flipper_ecu_sync_worker_process_ckp_tick, worker);
}

void flipper_ecu_sync_worker_await_stop(FlipperECUSyncWorker* worker) {
    furi_thread_join(worker->thread);
}
