#pragma once

#include <stdint.h>
#include <furi.h>

typedef enum {
    FlipperECUIgnitionWorkerEventStop = (1 << 0),
    FlipperECUIgnitionWorkerEventFire = (1 << 1),
    FlipperECUIgnitionWorkerEventAll = FlipperECUIgnitionWorkerEventStop |
                                       FlipperECUIgnitionWorkerEventFire
} FlipperECUIgnitionWorkerEvent;

int32_t flipper_ecu_ignition_worker_thread(void* arg);
void flipper_ecu_ignition_worker_thread_send_stop(FuriThread* thread);
