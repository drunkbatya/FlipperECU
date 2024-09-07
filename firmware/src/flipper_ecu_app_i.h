#pragma once

#include "flipper_ecu_app.h"

#include "gui/flipper_ecu_gui.h"
#include "flipper_ecu_engine_status.h"

struct FlipperECUApp {
    FlipperECUGui* gui;
    FlipperECUSyncWorker* sync_worker;
    FlipperECUAdcWorker* adc_worker;
    FlipperECUFuelPumpWorker* fuel_pump_worker;
    FlipperECUEngineStatus engine_status;
    FlipperECUEngineSettings* engine_settings;
};
