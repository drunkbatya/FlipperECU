#pragma once

#include <furi.h>
#include "gui/flipper_ecu_gui.h"
#include "worker/flipper_ecu_sync_worker.h"
#include "flipper_ecu_engine_adjustments.h"

typedef struct {
    FlipperECUGui* gui;
    FlipperECUSyncWorker* sync_worker;
    FlipperECUEngineAdjustments* adj;
} FlipperECUApp;

FlipperECUSyncWorker* flipper_ecu_app_get_sync_worker(FlipperECUApp* app);
