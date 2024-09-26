#pragma once

#include "flipper_ecu_fuel_pump_worker.h"

#define FUEL_PUMP_POWER_OFF_TIMEOUT_MS 3000
#define FUEL_PUMP_OFF_LEVEL 0
#define FUEL_PUMP_ON_LEVEL 1

struct FlipperECUFuelPumpWorker {
    FuriThread* thread;
    FlipperECUEngineStatus* engine_status;
};

typedef enum {
    FlipperECUFuelPumpWorkerEventStop = (1 << 0),
    FlipperECUFuelPumpWorkerEventEngineStopped = (1 << 1),
    FlipperECUFuelPumpWorkerEventIgnitionSwitchedOn = (1 << 2),
    FlipperECUFuelPumpWorkerEventEngineRunning = (1 << 3),
    FlipperECUFuelPumpWorkerEventAll = FlipperECUFuelPumpWorkerEventStop |
                                       FlipperECUFuelPumpWorkerEventEngineStopped |
                                       FlipperECUFuelPumpWorkerEventIgnitionSwitchedOn |
                                       FlipperECUFuelPumpWorkerEventEngineRunning
} FlipperECUFuelPumpWorkerEvent;
