#include "flipper_ecu_engine_settings.h"

FlipperECUEngineSettings* flipper_ecu_engine_settings_alloc(void) {
    FlipperECUEngineSettings* set = malloc(sizeof(FlipperECUEngineSettings));

    // allocating map set
    // Ignition test map
    set->maps[IGN_MAP] = flipper_ecu_map_create_alloc_2d(16);
    flipper_ecu_map_set_names_2d(set->maps[IGN_MAP], "Ignition main", "RPM", "Angle");
    flipper_ecu_map_set_ranges(set->maps[IGN_MAP], -15, 55);

    // Ignition by TPS map
    set->maps[IGN_TPS_MAP] = flipper_ecu_map_create_alloc_3d(16, 3);
    flipper_ecu_map_set_names_3d(
        set->maps[IGN_TPS_MAP], "Ignition by TPS", "RPM", "TPS %", "Angle");
    flipper_ecu_map_set_ranges(set->maps[IGN_TPS_MAP], -15, 55);

    return set;
}

void flipper_ecu_engine_settings_free(FlipperECUEngineSettings* set) {
    flipper_ecu_map_free(set->maps[IGN_MAP]);
    flipper_ecu_map_free(set->maps[IGN_TPS_MAP]);
    free(set);
}

//furi_string_set(set->file_path, STORAGE_APP_DATA_PATH_PREFIX);
//furi_string_cat_printf(set->file_path, "/%s%s", "fuckyou", ENGINE_SETTINGS_FILE_EXT);
void flipper_ecu_engine_settings_load_d(FlipperECUEngineSettings* set) {
    // debug 2D map
    const int16_t test_data[16] = {30, 30, 30, 30, 25, 23, 22, 17, 18, 29, 28, 27, 10, 9, 13, 20};
    const int16_t test_keys[16] = {
        600, 720, 840, 990, 1170, 1380, 1650, 1950, 2310, 2730, 3210, 3840, 4530, 5370, 6360, 7500};
    flipper_ecu_map_set_keys_x(set->maps[IGN_MAP], test_keys);
    flipper_ecu_map_set_values_2d(set->maps[IGN_MAP], test_data);

    // debug 3D map
    const int16_t test_data_3d[16 * 3] = {0,  40, 13, 12, 20, 15, 16, 17, 18, 29, 28, 27,
                                          10, 9,  13, 12, 2,  42, 15, 14, 22, 17, 18, 19,
                                          20, 31, 30, 29, 12, 11, 15, 14, 4,  44, 17, 16,
                                          24, 19, 20, 21, 22, 33, 32, 31, 14, 13, 17, 16};
    const int16_t test_keys_x_3d[16] = {
        600, 720, 840, 990, 1170, 1380, 1650, 1950, 2310, 2730, 3210, 3840, 4530, 5370, 6360, 7500};
    const int16_t test_keys_z_3d[3] = {30, 75, 98};

    flipper_ecu_map_set_keys_x(set->maps[IGN_TPS_MAP], test_keys_x_3d);
    flipper_ecu_map_set_keys_z_3d(set->maps[IGN_TPS_MAP], test_keys_z_3d);
    flipper_ecu_map_set_values_3d(set->maps[IGN_TPS_MAP], test_data_3d);
}
