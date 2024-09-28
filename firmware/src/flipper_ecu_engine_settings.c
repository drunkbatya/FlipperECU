#include "flipper_ecu_engine_settings.h"

FlipperECUEngineSettings* flipper_ecu_engine_settings_alloc(void) {
    FlipperECUEngineSettings* set = malloc(sizeof(FlipperECUEngineSettings));

    // allocating map set
    // Ignition test map
    set->maps[IGN_MAP] = flipper_ecu_map_create_alloc_2d(16);
    flipper_ecu_map_set_names_2d(set->maps[IGN_MAP], "Ignition main", "RPM", "Angle");
    flipper_ecu_map_set_ranges(set->maps[IGN_MAP], -15, 55);

    // Ignition by TPS map
    //set->maps[IGN_TPS_MAP] = flipper_ecu_map_create_alloc_3d(16, 3);
    //flipper_ecu_map_set_names_3d(
    //    set->maps[IGN_TPS_MAP], "Ignition by TPS", "RPM", "TPS %", "Angle");
    //flipper_ecu_map_set_ranges(set->maps[IGN_TPS_MAP], -15, 55);

    // TPS test map
    set->maps[TPS_TEST_MAP] = flipper_ecu_map_create_alloc_2d(16);
    flipper_ecu_map_set_names_2d(set->maps[TPS_TEST_MAP], "Inj time", "TPS %", "ms*10");
    flipper_ecu_map_set_ranges(set->maps[TPS_TEST_MAP], 0, 300);

    // injectors dead time
    set->maps[INJ_DEAD_TIME] = flipper_ecu_map_create_alloc_2d(32);
    flipper_ecu_map_set_names_2d(set->maps[INJ_DEAD_TIME], "Inj dead time", "Vbat V", "ms*100");
    flipper_ecu_map_set_ranges(set->maps[INJ_DEAD_TIME], -100, 700);

    // air temp sensor
    set->maps[AIR_TEMP_SENSOR] = flipper_ecu_map_create_alloc_2d(17);
    flipper_ecu_map_set_names_2d(set->maps[AIR_TEMP_SENSOR], "Temp sensor", "ADC V", "temp C");
    flipper_ecu_map_set_ranges(set->maps[AIR_TEMP_SENSOR], 0, 190);

    return set;
}

void flipper_ecu_engine_settings_free(FlipperECUEngineSettings* set) {
    flipper_ecu_map_free(set->maps[INJ_DEAD_TIME]);
    flipper_ecu_map_free(set->maps[AIR_TEMP_SENSOR]);
    flipper_ecu_map_free(set->maps[IGN_MAP]);
    flipper_ecu_map_free(set->maps[TPS_TEST_MAP]);
    free(set);
}

//furi_string_set(set->file_path, STORAGE_APP_DATA_PATH_PREFIX);
//furi_string_cat_printf(set->file_path, "/%s%s", "fuckyou", ENGINE_SETTINGS_FILE_EXT);
void flipper_ecu_engine_settings_load_d(FlipperECUEngineSettings* set) {
    // debug 2D map
    const int16_t test_data[16] = {30, 30, 30, 30, 30, 30, 30, 30, 18, 29, 28, 27, 10, 9, 13, 20};
    const int16_t test_keys[16] = {
        600, 720, 840, 990, 1170, 1380, 1650, 1950, 2310, 2730, 3210, 3840, 4530, 5370, 6360, 7500};
    flipper_ecu_map_set_keys_x(set->maps[IGN_MAP], test_keys);
    flipper_ecu_map_set_values_2d(set->maps[IGN_MAP], test_data);

    // debug 3D map
    //const int16_t test_data_3d[16 * 3] = {0,  40, 13, 12, 20, 15, 16, 17, 18, 29, 28, 27,
    //                                      10, 9,  13, 12, 2,  42, 15, 14, 22, 17, 18, 19,
    //                                      20, 31, 30, 29, 12, 11, 15, 14, 4,  44, 17, 16,
    //                                      24, 19, 20, 21, 22, 33, 32, 31, 14, 13, 17, 16};
    //const int16_t test_keys_x_3d[16] = {
    //    600, 720, 840, 990, 1170, 1380, 1650, 1950, 2310, 2730, 3210, 3840, 4530, 5370, 6360, 7500};
    //const int16_t test_keys_z_3d[3] = {30, 75, 98};

    //flipper_ecu_map_set_keys_x(set->maps[IGN_TPS_MAP], test_keys_x_3d);
    //flipper_ecu_map_set_keys_z_3d(set->maps[IGN_TPS_MAP], test_keys_z_3d);
    //flipper_ecu_map_set_values_3d(set->maps[IGN_TPS_MAP], test_data_3d);

    const int16_t tps_test_keys[16] = {0, 2, 4, 6, 8, 10, 14, 18, 23, 29, 37, 46, 56, 66, 80, 100};
    const int16_t tps_test_values[16] = {
        55, 67, 73, 88, 66, 70, 71, 74, 76, 77, 78, 78, 72, 70, 66, 55};
    flipper_ecu_map_set_keys_x(set->maps[TPS_TEST_MAP], tps_test_keys);
    flipper_ecu_map_set_values_2d(set->maps[TPS_TEST_MAP], tps_test_values);

    // inj dead time
    const int16_t inj_dead_time_values[32] = {580, 450, 380, 330, 300, 275, 250, 230,
                                              212, 200, 190, 182, 175, 167, 160, 150,
                                              145, 140, 134, 130, 126, 122, 118, 116,
                                              112, 110, 106, 104, 102, 99,  97,  96};
    const int16_t inj_dead_time_keys[32] = {5400,  5800,  6200,  6600,  7000,  7400,  7800,
                                            8200,  8600,  9000,  9400,  9800,  10200, 10600,
                                            11000, 11400, 11800, 12200, 12600, 13000, 13400,
                                            13800, 14200, 14600, 15000, 15400, 15800, 16200,
                                            16600, 17000, 17400, 17800};
    flipper_ecu_map_set_keys_x(set->maps[INJ_DEAD_TIME], inj_dead_time_keys);
    flipper_ecu_map_set_values_2d(set->maps[INJ_DEAD_TIME], inj_dead_time_values);

    // temp sensor
    const int16_t air_temp_sensor_values[17] = {
        190, 148, 114, 94, 80, 69, 60, 52, 44, 36, 29, 22, 14, 5, -6, -24, -51};
    const int16_t air_temp_sensor_keys[17] = {
        0,
        311,
        622,
        934,
        1245,
        1556,
        1867,
        2179,
        2490,
        2801,
        3112,
        3424,
        3735,
        4046,
        4357,
        4669,
        4980};
    flipper_ecu_map_set_keys_x(set->maps[AIR_TEMP_SENSOR], air_temp_sensor_keys);
    flipper_ecu_map_set_values_2d(set->maps[AIR_TEMP_SENSOR], air_temp_sensor_values);

    // maps end

    set->idle_valve_pwm_freq = 500;
    set->idle_valve_total_steps = 255;

    set->idle_valve_position_on_ignition_on = 70;

    set->cranking_end_rpm = 500;
}
