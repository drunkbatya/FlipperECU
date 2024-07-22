#include "flipper_ecu_engine_adjustments.h"

FlipperECUEngineAdjustments* flipper_ecu_engine_adj_alloc(void) {
    FlipperECUEngineAdjustments* adj = malloc(sizeof(FlipperECUEngineAdjustments));

    // debug 2D map
    const int16_t test_data[16] = {0, 40, 13, 12, 20, 15, 16, 17, 18, 29, 28, 27, 10, 9, 13, 20};
    const int16_t test_keys[16] = {
        600, 720, 840, 990, 1170, 1380, 1650, 1950, 2310, 2730, 3210, 3840, 4530, 5370, 6360, 7500};
    adj->ign_map = flipper_ecu_map_create_alloc(FlipperECUMapType2D, 16, 0);
    flipper_ecu_map_set_names_2d(adj->ign_map, "Ignition main", "RPM", "Angle");
    flipper_ecu_map_set_ranges(adj->ign_map, -15, 55);
    flipper_ecu_map_set_keys_x(adj->ign_map, test_keys);
    flipper_ecu_map_set_values_2d(adj->ign_map, test_data);

    // debug 3D map
    const int16_t test_data_3d[16 * 3] = {0,  40, 13, 12, 20, 15, 16, 17, 18, 29, 28, 27,
                                          10, 9,  13, 12, 2,  42, 15, 14, 22, 17, 18, 19,
                                          20, 31, 30, 29, 12, 11,  15, 14, 4,  44, 17, 16,
                                          24, 19, 20, 21, 22, 33, 32, 31, 14, 13,  17, 16};
    const int16_t test_keys_x_3d[16] = {
        600, 720, 840, 990, 1170, 1380, 1650, 1950, 2310, 2730, 3210, 3840, 4530, 5370, 6360, 7500};
    const int16_t test_keys_z_3d[3] = {30, 75, 98};

    adj->ign_tps_map = flipper_ecu_map_create_alloc(FlipperECUMapType3D, 16, 3);
    flipper_ecu_map_set_names_3d(adj->ign_tps_map, "Ignition by TPS", "RPM", "TPS %", "Angle");
    flipper_ecu_map_set_ranges(adj->ign_tps_map, -15, 55);
    flipper_ecu_map_set_keys_x(adj->ign_tps_map, test_keys_x_3d);
    flipper_ecu_map_set_keys_z_3d(adj->ign_tps_map, test_keys_z_3d);
    flipper_ecu_map_set_values_3d(adj->ign_tps_map, test_data_3d);

    return adj;
}

void flipper_ecu_engine_adj_free(FlipperECUEngineAdjustments* adj) {
    flipper_ecu_map_free(adj->ign_map);
    flipper_ecu_map_free(adj->ign_tps_map);
    free(adj);
}
