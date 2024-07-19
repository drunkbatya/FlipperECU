#pragma once

#include "flipper_ecu_map.h"

typedef struct {
    FlipperECUMap* ign_map;
    FlipperECUMap* ign_tps_map;
} FlipperECUEngineAdjustments;

FlipperECUEngineAdjustments* flipper_ecu_engine_adj_alloc(void);
void flipper_ecu_engine_adj_free(FlipperECUEngineAdjustments* adj);
