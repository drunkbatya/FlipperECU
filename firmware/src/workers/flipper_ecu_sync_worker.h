#pragma once

#include <stdint.h>
#include <furi.h>
#include "../flipper_ecu_settings.h"
#include "../flipper_ecu_gpio.h"

typedef struct {
    FuriThread* thread;
    FlipperECUSettings* settings;
    const FlipperECUGPIO* gpio;
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
