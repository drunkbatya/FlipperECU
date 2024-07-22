#pragma once

#include <storage/storage.h>

#include "flipper_ecu_map.h"

typedef struct {
    FlipperECUMap* ign_map;
    FlipperECUMap* ign_tps_map;
} FlipperECUEngineAdjustments;

typedef struct {
    FlipperECUEngineAdjustments adj;
} FlipperECUEngineSettings;

void flipper_ecu_engine_settings_load(FlipperECUEngineSettings* set);
FlipperECUEngineSettings* flipper_ecu_engine_settings_alloc(void);
void flipper_ecu_engine_settings_free(FlipperECUEngineSettings* set);
