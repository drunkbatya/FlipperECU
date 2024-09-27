#pragma once

#include <furi.h>
#include <storage/storage.h>

#include "flipper_ecu_map.h"

#define ENGINE_SETTINGS_FILE_EXT ".ass"

typedef enum {
    IGN_MAP,
    IGN_TPS_MAP,
    MAF_DECODE_MAP,
    TPS_TEST_MAP,
    MAP_COUNT
} FlipperECUEngineSettingsMaps;

typedef struct {
    FlipperECUMap* maps[MAP_COUNT];

    // idle valve
    uint16_t idle_valve_total_steps;
    uint16_t idle_valve_pwm_freq;

    uint16_t idle_valve_position_on_ignition_on;
} FlipperECUEngineSettings;

void flipper_ecu_engine_settings_load_d(FlipperECUEngineSettings* set);
FlipperECUEngineSettings* flipper_ecu_engine_settings_alloc(void);
void flipper_ecu_engine_settings_free(FlipperECUEngineSettings* set);
