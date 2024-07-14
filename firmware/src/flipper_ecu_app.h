#pragma once

#include <furi.h>
#include "gui/flipper_ecu_gui.h"
#include "worker/flipper_ecu_sync_worker.h"

typedef struct {
    FlipperECUGui* gui;
    FlipperECUSyncWorker* sync_worker;
} FlipperECUApp;

FlipperECUSyncWorker* flipper_ecu_app_get_sync_worker(FlipperECUApp* app);
