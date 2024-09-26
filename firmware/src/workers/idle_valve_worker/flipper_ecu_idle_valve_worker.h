#pragma once

#include <furi.h>

typedef struct FlipperECUIdleValveWorker FlipperECUIdleValveWorker;

#include "../../flipper_ecu_engine_settings.h"

typedef enum { IdleValveDirectionForward, IdleValveDirectionBackward } IdleValveDirection;

FlipperECUIdleValveWorker*
    flipper_ecu_idle_valve_worker_alloc(FlipperECUEngineSettings* engine_settings);
void flipper_ecu_idle_valve_worker_start(FlipperECUIdleValveWorker* worker);
void flipper_ecu_idle_valve_worker_free(FlipperECUIdleValveWorker* worker);
void flipper_ecu_idle_valve_worker_send_stop(FlipperECUIdleValveWorker* worker);
void flipper_ecu_idle_valve_worker_await_stop(FlipperECUIdleValveWorker* worker);
void flipper_ecu_idle_valve_worker_calibrate(FlipperECUIdleValveWorker* worker);
void flipper_ecu_idle_valve_worker_step(
    FlipperECUIdleValveWorker* worker,
    IdleValveDirection direction);
uint16_t flipper_ecu_idle_valve_worker_get_current_position(FlipperECUIdleValveWorker* worker);
void flipper_ecu_idle_valve_worker_notify_ignition_switched_on(FlipperECUIdleValveWorker* worker);
void flipper_ecu_idle_valve_worker_move_to(FlipperECUIdleValveWorker* worker, uint16_t position);
