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

    // MAF decode map
    set->maps[MAF_DECODE_MAP] = flipper_ecu_map_create_alloc_2d(256);
    flipper_ecu_map_set_names_2d(set->maps[MAF_DECODE_MAP], "MAF decode", "mV", "kg/h");
    flipper_ecu_map_set_ranges(set->maps[MAF_DECODE_MAP], -384, 7997);

    // TPS test map
    set->maps[TPS_TEST_MAP] = flipper_ecu_map_create_alloc_2d(16);
    flipper_ecu_map_set_names_2d(set->maps[TPS_TEST_MAP], "Inj time", "TPS %", "ms*10");
    flipper_ecu_map_set_ranges(set->maps[TPS_TEST_MAP], 0, 300);

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
    const int16_t test_data[16] = {30, 30, 30, 30, 30, 30, 30, 30, 18, 29, 28, 27, 10, 9, 13, 20};
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

    // maf debug
    const int16_t maf_keys[256] = {
        0,    19,   39,   58,   78,   97,   117,  136,  156,  175,  195,  214,  234,  253,  273,
        292,  312,  332,  351,  371,  390,  410,  429,  449,  468,  488,  507,  527,  546,  566,
        585,  605,  624,  644,  664,  683,  703,  722,  742,  761,  781,  800,  820,  839,  859,
        878,  898,  917,  937,  956,  976,  996,  1015, 1035, 1054, 1074, 1093, 1113, 1132, 1152,
        1171, 1191, 1210, 1230, 1249, 1269, 1288, 1308, 1328, 1347, 1367, 1386, 1406, 1425, 1445,
        1464, 1484, 1503, 1523, 1542, 1562, 1581, 1601, 1620, 1640, 1660, 1679, 1699, 1718, 1738,
        1757, 1777, 1796, 1816, 1835, 1855, 1874, 1894, 1913, 1933, 1952, 1972, 1992, 2011, 2031,
        2050, 2070, 2089, 2109, 2128, 2148, 2167, 2187, 2206, 2226, 2245, 2265, 2284, 2304, 2324,
        2343, 2363, 2382, 2402, 2421, 2441, 2460, 2480, 2499, 2519, 2538, 2558, 2577, 2597, 2616,
        2636, 2656, 2675, 2695, 2714, 2734, 2753, 2773, 2792, 2812, 2831, 2851, 2870, 2890, 2909,
        2929, 2948, 2968, 2988, 3007, 3027, 3046, 3066, 3085, 3105, 3124, 3144, 3163, 3183, 3202,
        3222, 3241, 3261, 3280, 3300, 3320, 3339, 3359, 3378, 3398, 3417, 3437, 3456, 3476, 3495,
        3515, 3534, 3554, 3573, 3593, 3612, 3632, 3652, 3671, 3691, 3710, 3730, 3749, 3769, 3788,
        3808, 3827, 3847, 3866, 3886, 3905, 3925, 3944, 3964, 3984, 4003, 4023, 4042, 4062, 4081,
        4101, 4120, 4140, 4159, 4179, 4198, 4218, 4237, 4257, 4276, 4296, 4316, 4335, 4355, 4374,
        4394, 4413, 4433, 4452, 4472, 4491, 4511, 4530, 4550, 4569, 4589, 4608, 4628, 4648, 4667,
        4687, 4706, 4726, 4745, 4765, 4784, 4804, 4823, 4843, 4862, 4882, 4901, 4921, 4940, 4960,
        4980};
    const int16_t maf_values[256] = {
        -384, -373, -362, -351, -340, -330, -319, -309, -299, -289, -279, -270, -261, -251, -243,
        -234, -225, -217, -208, -200, -193, -185, -177, -170, -163, -156, -149, -142, -136, -129,
        -123, -117, -111, -105, -100, -94,  -89,  -83,  -78,  -73,  -67,  -62,  -57,  -51,  -45,
        -39,  -33,  -27,  -20,  -14,  -7,   0,    6,    12,   19,   25,   31,   37,   44,   50,
        56,   62,   68,   73,   78,   83,   89,   94,   100,  105,  111,  117,  123,  129,  136,
        142,  149,  156,  163,  170,  177,  185,  192,  200,  208,  217,  225,  234,  242,  251,
        261,  270,  279,  289,  299,  309,  319,  329,  339,  349,  359,  370,  381,  391,  402,
        414,  425,  436,  447,  460,  472,  485,  499,  512,  526,  540,  554,  569,  584,  599,
        615,  631,  647,  664,  680,  697,  715,  733,  751,  769,  788,  807,  826,  846,  866,
        886,  906,  927,  949,  970,  992,  1015, 1037, 1060, 1084, 1107, 1131, 1155, 1179, 1204,
        1229, 1255, 1282, 1309, 1336, 1364, 1392, 1421, 1451, 1481, 1512, 1544, 1576, 1608, 1641,
        1675, 1709, 1744, 1779, 1815, 1852, 1889, 1926, 1965, 2004, 2043, 2083, 2124, 2165, 2207,
        2250, 2294, 2338, 2383, 2429, 2475, 2522, 2570, 2619, 2669, 2719, 2770, 2822, 2875, 2928,
        2982, 3037, 3092, 3149, 3206, 3264, 3323, 3383, 3444, 3505, 3567, 3630, 3694, 3759, 3825,
        3892, 3960, 4029, 4098, 4168, 4239, 4311, 4383, 4457, 4533, 4610, 4688, 4767, 4847, 4928,
        5010, 5093, 5178, 5263, 5350, 5438, 5527, 5616, 5707, 5800, 5893, 5987, 6083, 6179, 6277,
        6376, 6475, 6576, 6678, 6780, 6884, 6989, 7095, 7202, 7311, 7422, 7534, 7648, 7763, 7879,
        7997};
    flipper_ecu_map_set_keys_x(set->maps[MAF_DECODE_MAP], maf_keys);
    flipper_ecu_map_set_values_2d(set->maps[MAF_DECODE_MAP], maf_values);

    const int16_t tps_test_keys[16] = {0, 2, 4, 6, 8, 10, 14, 18, 23, 29, 37, 46, 56, 66, 80, 100};
    const int16_t tps_test_values[16] = {
        55, 67, 73, 88, 66, 70, 71, 74, 76, 77, 78, 78, 72, 70, 66, 55};
    flipper_ecu_map_set_keys_x(set->maps[TPS_TEST_MAP], tps_test_keys);
    flipper_ecu_map_set_values_2d(set->maps[TPS_TEST_MAP], tps_test_values);

    set->idle_valve_pwm_freq = 500;
    set->idle_valve_total_steps = 255;

    set->idle_valve_position_on_ignition_on = 70;
}
