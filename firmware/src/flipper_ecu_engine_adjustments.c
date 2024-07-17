#include "flipper_ecu_engine_adjustments.h"

FlipperECUEngineAdjustments* flipper_ecu_engine_adj_alloc(void) {
    FlipperECUEngineAdjustments* adj = malloc(sizeof(FlipperECUEngineAdjustments));

    // debug
    const int16_t test_data[16] = {0, 40, 13, 12, 20, 15, 16, 17, 18, 29, 28, 27, 10, 9, 13};
    const int16_t test_keys[16] = {
        600, 720, 840, 990, 1170, 1380, 1650, 1950, 2310, 2730, 3210, 3840, 4530, 5370, 6360, 7500};
    adj->ign_map = flipper_ecu_map_create_alloc(FlipperECUMapType2D, 16, 0);
    flipper_ecu_map_set_names(adj->ign_map, "Ignition main map", "> RPM", NULL, "^ Angle");
    flipper_ecu_map_set_ranges(adj->ign_map, -15, 55);
    flipper_ecu_map_set_keys(adj->ign_map, test_keys);
    flipper_ecu_map_set_values(adj->ign_map, test_data);

    return adj;
}

void flipper_ecu_engine_adj_free(FlipperECUEngineAdjustments* adj) {
    flipper_ecu_map_free(adj->ign_map);
    free(adj);
}
