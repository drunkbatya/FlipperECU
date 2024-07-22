#pragma once

#include <furi.h>
#include <storage/storage.h>

#include "flipper_ecu_map.h"

typedef struct {
    FlipperECUMap* ign_map;
    FlipperECUMap* ign_tps_map;
} FlipperECUEngineSettingsMaps;

typedef struct {
    FlipperECUEngineSettingsMaps maps;
    FuriString* file_path;
} FlipperECUEngineSettings;

void flipper_ecu_engine_settings_load_d(FlipperECUEngineSettings* set);
FlipperECUEngineSettings* flipper_ecu_engine_settings_alloc(void);
void flipper_ecu_engine_settings_free(FlipperECUEngineSettings* set);
bool flipper_ecu_engine_settings_save(FlipperECUEngineSettings* set);
