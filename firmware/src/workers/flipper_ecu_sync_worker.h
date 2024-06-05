#pragma once

#include <stdint.h>
#include <furi.h>
#include "../flipper_ecu_settings.h"
#include "../flipper_ecu_gpio.h"

typedef struct {
    FuriThread* thread;
    FlipperECUSettings* settings;
    const FlipperECUGPIO* gpio;
    uint32_t current_tick;
    uint32_t previous_tick;
    uint32_t timer;
    bool synced;
} FlipperECUSyncWorker;

typedef enum {
    FlipperECUSyncWorkerEventStop = (1 << 0),
    FlipperECUSyncWorkerEventCkpPulse = (1 << 1),
    FlipperECUSyncWorkerEventAll = FlipperECUSyncWorkerEventStop |
                                   FlipperECUSyncWorkerEventCkpPulse
} FlipperECUSyncWorkerEvent;

FlipperECUSyncWorker*
    flipper_ecu_sync_worker_alloc(FlipperECUSettings* settings, const FlipperECUGPIO* gpio);
void flipper_ecu_sync_worker_send_stop(FlipperECUSyncWorker* worker);
void flipper_ecu_sync_worker_await_stop(FlipperECUSyncWorker* worker);
void flipper_ecu_sync_worker_start(FlipperECUSyncWorker* worker);
void flipper_ecu_sync_worker_free(FlipperECUSyncWorker* worker);
uint32_t flipper_ecu_sync_worker_get_rpm(FlipperECUSyncWorker* worker);