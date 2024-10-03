#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    EngineModeIgnitionOff,
    EngineModeStopped,
    EngineModeCranking,
    EngineModeIdle,
    EngineModeWorking
} EngineMode;

typedef struct {
    uint16_t rpm;
    int16_t ign_angle;
    bool synced;
    double inj_time;
    double inj_dead_time;
    uint32_t sync_processing_time;
    uint32_t first_queue_event_ticks;
    double t_charge;
    double ve;
    double air_mass;
    double manifold_pressure;
    EngineMode mode;
} FlipperECUEngineStatus;

const char* flipper_ecu_engine_status_get_mode_name(const FlipperECUEngineStatus* status);
