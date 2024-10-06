#pragma once

#include "flipper_ecu_idle_valve_worker.h"

#define FUEL_PUMP_POWER_OFF_TIMEOUT_MS 1000
#define FUEL_PUMP_OFF_LEVEL 0
#define FUEL_PUMP_ON_LEVEL 1

struct FlipperECUIdleValveWorker {
    FuriThread* thread;
    FlipperECUApp* ecu_app;
    FlipperECUEngineSettings* engine_settings;
    FlipperECUEngineStatus* engine_status;
    IdleValveDirection direction;
    bool calibration_ongoing;
    bool calibration_done;
    bool movement_done;
    uint16_t current_position;
    uint16_t stop_timer_max_overflow_count;
    uint16_t stop_timer_last_reload;
    uint16_t stop_timer_current_overflow_count;
};

typedef enum {
    FlipperECUIdleValveWorkerEventStop = (1 << 0),
    FlipperECUIdleValveWorkerEventEngineStopped = (1 << 1),
    FlipperECUIdleValveWorkerEventIgnitionSwitchedOn = (1 << 2),
    FlipperECUIdleValveWorkerEventEngineRunning = (1 << 3),
    FlipperECUIdleValveWorkerEventAll = FlipperECUIdleValveWorkerEventStop |
                                        FlipperECUIdleValveWorkerEventEngineStopped |
                                        FlipperECUIdleValveWorkerEventIgnitionSwitchedOn |
                                        FlipperECUIdleValveWorkerEventEngineRunning
} FlipperECUIdleValveWorkerEvent;
