#pragma once

#include <stdint.h>
#include <furi.h>
#include "../flipper_ecu_settings.h"
#include "../flipper_ecu_gpio.h"

typedef struct {
    FuriThread* thread;
    FlipperECUSettings* settings;
    const FlipperECUGPIO* gpio;
    uint32_t timer_overflows;
    uint32_t current_period;
    uint32_t previous_period;
    uint32_t temp;
    bool synced;
} FlipperECUSyncWorker;

typedef enum {
    FlipperECUSyncWorkerEventStop = (1 << 0),
    FlipperECUSyncWorkerEventCkpPulse = (1 << 1),
    FlipperECUSyncWorkerEventPredictionDone = (1 << 2),
    FlipperECUSyncWorkerEventAll = FlipperECUSyncWorkerEventStop |
                                   FlipperECUSyncWorkerEventCkpPulse |
                                   FlipperECUSyncWorkerEventPredictionDone
} FlipperECUSyncWorkerEvent;

FlipperECUSyncWorker*
    flipper_ecu_sync_worker_alloc(FlipperECUSettings* settings, const FlipperECUGPIO* gpio);
void flipper_ecu_sync_worker_send_stop(FlipperECUSyncWorker* worker);
void flipper_ecu_sync_worker_await_stop(FlipperECUSyncWorker* worker);
void flipper_ecu_sync_worker_start(FlipperECUSyncWorker* worker);
void flipper_ecu_sync_worker_free(FlipperECUSyncWorker* worker);
uint32_t flipper_ecu_sync_worker_get_rpm(FlipperECUSyncWorker* worker);
