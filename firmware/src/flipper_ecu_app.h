#pragma once

#include <furi.h>
#include "gui/flipper_ecu_gui.h"
#include "workers/flipper_ecu_sync_worker.h"
#include "flipper_ecu_settings.h"
#include "flipper_ecu_gpio.h"

typedef struct {
    FuriThread* gui_thread;
    FlipperECUSyncWorker* sync_worker;
    FlipperECUSettings* settings;
    const FlipperECUGPIO* gpio;
} FlipperECUApp;
