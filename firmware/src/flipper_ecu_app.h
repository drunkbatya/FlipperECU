#pragma once

#include <furi.h>
#include "gui/flipper_ecu_gui.h"
#include "worker/flipper_ecu_sync_worker.h"
#include "flipper_ecu_settings.h"

typedef struct {
    FlipperECUGui* gui;
    FlipperECUSyncWorker* sync_worker;
    FlipperECUSettings* settings;
} FlipperECUApp;

typedef enum {
    CKPSPolatityRasing,
    CKPSPolatityFalling
} CKPSPolatity;
