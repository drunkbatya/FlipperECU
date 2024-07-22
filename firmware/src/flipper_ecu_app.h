#pragma once

#include <furi.h>
#include "gui/flipper_ecu_gui.h"
#include "worker/flipper_ecu_sync_worker.h"

#include "flipper_ecu_engine_status.h"

typedef struct {
    FlipperECUGui* gui;
    FlipperECUSyncWorker* sync_worker;
    FlipperECUEngineStatus engine_status;
    FlipperECUEngineSettings* engine_settings;
} FlipperECUApp;

FlipperECUSyncWorker* flipper_ecu_app_get_sync_worker(FlipperECUApp* app);
