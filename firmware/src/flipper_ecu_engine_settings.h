#pragma once

#include <furi.h>
#include <storage/storage.h>

#include "flipper_ecu_map.h"

#define ENGINE_SETTINGS_FILE_EXT ".ass"

typedef enum {
    INJ_DEAD_TIME,
    TEMP_SENSOR,
    INJ_PULSE_WIDTH_CRANKING,
    IGN_ANGLE_CRANKING,
    IGN_ANGLE_IDLE,
    VE,
    IGN_MAP,
    MAP_COUNT
} FlipperECUEngineSettingsMaps;

typedef struct {
    FlipperECUMap* maps[MAP_COUNT];

    // engine
    uint32_t engine_displacement;
    uint8_t cylinders_count;

    // constants
    double idle_tps_value;

    // injectors
    double inj_flow;

    // idle valve
    uint16_t idle_valve_total_steps;
    uint16_t idle_valve_pwm_freq;

    // cranking
    uint16_t idle_valve_position_on_ignition_on;
    uint16_t cranking_end_rpm;
} FlipperECUEngineSettings;

void flipper_ecu_engine_settings_load_d(FlipperECUEngineSettings* set);
FlipperECUEngineSettings* flipper_ecu_engine_settings_alloc(void);
void flipper_ecu_engine_settings_free(FlipperECUEngineSettings* set);
