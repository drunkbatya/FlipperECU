#pragma once

#include <furi.h>

typedef struct FlipperECUFuelPumpWorker FlipperECUFuelPumpWorker;

#include "../../flipper_ecu_engine_status.h"

FlipperECUFuelPumpWorker*
    flipper_ecu_fuel_pump_worker_alloc(FlipperECUEngineStatus* engine_status);
void flipper_ecu_fuel_pump_worker_start(FlipperECUFuelPumpWorker* worker);
void flipper_ecu_fuel_pump_worker_free(FlipperECUFuelPumpWorker* worker);
void flipper_ecu_fuel_pump_worker_send_stop(FlipperECUFuelPumpWorker* worker);
void flipper_ecu_fuel_pump_worker_await_stop(FlipperECUFuelPumpWorker* worker);
void flipper_ecu_fuel_pump_notify_ignition_switched_on(FlipperECUFuelPumpWorker* worker);
void flipper_ecu_fuel_pump_notify_ignition_engine_stopped(FlipperECUFuelPumpWorker* worker);
void flipper_ecu_fuel_pump_notify_ignition_engine_running(FlipperECUFuelPumpWorker* worker);
