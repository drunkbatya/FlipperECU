#pragma once

typedef struct FlipperECUApp FlipperECUApp;

#include <furi.h>

#include "workers/sync_worker/flipper_ecu_sync_worker.h"
#include "workers/adc_worker/flipper_ecu_adc_worker.h"
#include "workers/fuel_pump_worker/flipper_ecu_fuel_pump_worker.h"
#include "workers/idle_valve_worker/flipper_ecu_idle_valve_worker.h"

FlipperECUSyncWorker* flipper_ecu_app_get_sync_worker(FlipperECUApp* app);
FlipperECUAdcWorker* flipper_ecu_app_get_adc_worker(FlipperECUApp* app);
FlipperECUFuelPumpWorker* flipper_ecu_app_get_fuel_pump_worker(FlipperECUApp* app);
FlipperECUIdleValveWorker* flipper_ecu_app_get_idle_valve_worker(FlipperECUApp* app);
FlipperECUEngineSettings* flipper_ecu_app_get_engine_settings(FlipperECUApp* app);
