#pragma once

#include <stdint.h>
#include <furi.h>

typedef struct FlipperECUSyncWorker FlipperECUSyncWorker;

#include "../../flipper_ecu_app.h"
#include "../../flipper_ecu_engine_config.h"
#include "../../flipper_ecu_engine_status.h"
#include "../../flipper_ecu_engine_settings.h"

FlipperECUSyncWorker* flipper_ecu_sync_worker_alloc(
    FlipperECUApp* ecu_app,
    FlipperECUEngineStatus* engine_status,
    FlipperECUEngineSettings* engine_settings);
void flipper_ecu_sync_worker_send_stop(FlipperECUSyncWorker* worker);
void flipper_ecu_sync_worker_await_stop(FlipperECUSyncWorker* worker);
void flipper_ecu_sync_worker_start(FlipperECUSyncWorker* worker);
void flipper_ecu_sync_worker_free(FlipperECUSyncWorker* worker);
uint32_t flipper_ecu_sync_worker_get_rpm(FlipperECUSyncWorker* worker);
const FlipperECUEngineConfig* flipper_ecu_sync_worker_get_config(FlipperECUSyncWorker* worker);
void flipper_ecu_sync_worker_update_config_restart(
    FlipperECUSyncWorker* worker,
    const FlipperECUEngineConfig* config);
void flipper_ecu_sync_worker_load_config(
    FlipperECUSyncWorker* worker,
    const FlipperECUEngineConfig* config);
void flipper_ecu_sync_worker_notify_ignition_switched_on(FlipperECUSyncWorker* worker);
