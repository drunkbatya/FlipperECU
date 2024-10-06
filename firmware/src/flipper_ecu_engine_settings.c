#include "flipper_ecu_engine_settings.h"

FlipperECUEngineSettings* flipper_ecu_engine_settings_alloc(void) {
    FlipperECUEngineSettings* set = malloc(sizeof(FlipperECUEngineSettings));

    // allocating map set
    // Ignition test map
    set->maps[IGN_MAP] = flipper_ecu_map_create_alloc_2d(16);
    flipper_ecu_map_set_names_2d(set->maps[IGN_MAP], "Ignition main", "RPM", "Angle");
    flipper_ecu_map_set_ranges(set->maps[IGN_MAP], -15, 55);

    // injectors dead time
    set->maps[INJ_DEAD_TIME] = flipper_ecu_map_create_alloc_2d(32);
    flipper_ecu_map_set_names_2d(set->maps[INJ_DEAD_TIME], "Inj dead time", "Vbat V", "ms*100");
    flipper_ecu_map_set_ranges(set->maps[INJ_DEAD_TIME], -100, 700);

    // air temp sensor
    set->maps[TEMP_SENSOR] = flipper_ecu_map_create_alloc_2d(32);
    flipper_ecu_map_set_names_2d(set->maps[TEMP_SENSOR], "Temp sensor", "ADC V", "Temp C");
    flipper_ecu_map_set_ranges(set->maps[TEMP_SENSOR], -40, 180);

    // injection time on cranking
    set->maps[INJ_PULSE_WIDTH_CRANKING] = flipper_ecu_map_create_alloc_2d(16);
    flipper_ecu_map_set_names_2d(
        set->maps[INJ_PULSE_WIDTH_CRANKING], "Inj pw cranking", "Temp C", "ms");
    flipper_ecu_map_set_ranges(set->maps[INJ_PULSE_WIDTH_CRANKING], 0, 1000);

    // main VE map
    set->maps[VE] = flipper_ecu_map_create_alloc_3d(16, 16);
    flipper_ecu_map_set_names_3d(set->maps[VE], "VE", "rpm", "air flow", "VE");
    flipper_ecu_map_set_ranges(set->maps[VE], 0, 1000);

    // Ignition map cranking
    set->maps[IGN_ANGLE_CRANKING] = flipper_ecu_map_create_alloc_2d(16);
    flipper_ecu_map_set_names_2d(
        set->maps[IGN_ANGLE_CRANKING], "Ign angle cranking", "RPM", "Angle");
    flipper_ecu_map_set_ranges(set->maps[IGN_ANGLE_CRANKING], -15, 55);

    // Ignition map idle
    set->maps[IGN_ANGLE_IDLE] = flipper_ecu_map_create_alloc_2d(16);
    flipper_ecu_map_set_names_2d(set->maps[IGN_ANGLE_IDLE], "Ign angle idle", "rpm", "angle");
    flipper_ecu_map_set_ranges(set->maps[IGN_ANGLE_IDLE], -15, 55);

    // injection afterstart enrichment
    set->maps[INJ_AFTERSTART_ENRICHMENT] = flipper_ecu_map_create_alloc_2d(16);
    flipper_ecu_map_set_names_2d(
        set->maps[INJ_AFTERSTART_ENRICHMENT], "Inj afterstart enrichment", "Temp C", "%");
    flipper_ecu_map_set_ranges(set->maps[INJ_AFTERSTART_ENRICHMENT], 0, 1800);

    // injection warmup enrichment
    set->maps[INJ_WARMUP_ENRICHMENT] = flipper_ecu_map_create_alloc_2d(16);
    flipper_ecu_map_set_names_2d(
        set->maps[INJ_WARMUP_ENRICHMENT], "Inj warmup enrichment", "temp C", "%");
    flipper_ecu_map_set_ranges(set->maps[INJ_WARMUP_ENRICHMENT], 0, 1800);

    // idle rpm
    set->maps[IDLE_RPM] = flipper_ecu_map_create_alloc_2d(16);
    flipper_ecu_map_set_names_2d(set->maps[IDLE_RPM], "Idle RPM", "temp C", "rpm");
    flipper_ecu_map_set_ranges(set->maps[IDLE_RPM], 0, 2400);

    return set;
}

void flipper_ecu_engine_settings_free(FlipperECUEngineSettings* set) {
    flipper_ecu_map_free(set->maps[INJ_DEAD_TIME]);
    flipper_ecu_map_free(set->maps[TEMP_SENSOR]);
    flipper_ecu_map_free(set->maps[IGN_MAP]);
    flipper_ecu_map_free(set->maps[INJ_PULSE_WIDTH_CRANKING]);
    flipper_ecu_map_free(set->maps[VE]);
    flipper_ecu_map_free(set->maps[IGN_ANGLE_CRANKING]);
    flipper_ecu_map_free(set->maps[IGN_ANGLE_IDLE]);
    flipper_ecu_map_free(set->maps[INJ_AFTERSTART_ENRICHMENT]);
    flipper_ecu_map_free(set->maps[INJ_WARMUP_ENRICHMENT]);
    flipper_ecu_map_free(set->maps[IDLE_RPM]);
    free(set);
}

//furi_string_set(set->file_path, STORAGE_APP_DATA_PATH_PREFIX);
//furi_string_cat_printf(set->file_path, "/%s%s", "fuckyou", ENGINE_SETTINGS_FILE_EXT);
void flipper_ecu_engine_settings_load_d(FlipperECUEngineSettings* set) {
    // debug 2D map
    const int16_t test_data[16] = {30, 30, 30, 30, 30, 30, 30, 30, 30, 29, 28, 27, 26, 28, 28, 30};
    const int16_t test_keys[16] = {
        600, 720, 840, 990, 1170, 1380, 1650, 1950, 2310, 2730, 3210, 3840, 4530, 5370, 6360, 7500};
    flipper_ecu_map_set_keys_x(set->maps[IGN_MAP], test_keys);
    flipper_ecu_map_set_values_2d(set->maps[IGN_MAP], test_data);

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
    const int16_t temp_sensor_values[32] = {175, 130, 106, 91, 82, 74, 68, 62,  58,  54, 50,
                                            46,  43,  40,  37, 34, 31, 28, 25,  23,  20, 17,
                                            14,  10,  7,   4,  0,  -4, -9, -15, -22, -37};
    const int16_t temp_sensor_keys[32] = {0,    161,  321,  482,  643,  803,  964,  1125,
                                          1285, 1446, 1606, 1767, 1928, 2088, 2249, 2410,
                                          2570, 2731, 2892, 3052, 3213, 3374, 3534, 3695,
                                          3855, 4016, 4177, 4337, 4498, 4659, 4819, 4980};
    flipper_ecu_map_set_keys_x(set->maps[TEMP_SENSOR], temp_sensor_keys);
    flipper_ecu_map_set_values_2d(set->maps[TEMP_SENSOR], temp_sensor_values);

    // injectors pulse width cranking
    const int16_t inj_pulse_width_cranking_values[16] = {
        102, 93, 86, 80, 73, 66, 60, 48, 42, 30, 30, 29, 26, 23, 22};
    const int16_t inj_pulse_width_cranking_keys[16] = {
        -30, -20, -10, 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120};
    flipper_ecu_map_set_keys_x(set->maps[INJ_PULSE_WIDTH_CRANKING], inj_pulse_width_cranking_keys);
    flipper_ecu_map_set_values_2d(
        set->maps[INJ_PULSE_WIDTH_CRANKING], inj_pulse_width_cranking_values);

    // VE table
    const int16_t ve_values[16 * 16] = {
        207, 200, 193, 186, 180, 173, 166, 159, 152, 145, 138, 132, 125, 118, 111, 104, // aa
        212, 205, 198, 192, 185, 178, 171, 165, 158, 151, 144, 138, 131, 124, 117, 111, // aa
        217, 210, 203, 165, 165, 183, 177, 170, 163, 157, 150, 143, 137, 130, 124, 117, // aa
        146, 168, 163, 164, 159, 162, 178, 176, 169, 163, 156, 149, 143, 136, 130, 123, // aa
        149, 165, 168, 166, 185, 172, 188, 181, 175, 168, 162, 155, 149, 143, 136, 130, // aa
        151, 146, 214, 197, 206, 186, 190, 184, 180, 174, 168, 161, 155, 149, 142, 136, // aa
        184, 166, 190, 197, 211, 205, 198, 192, 186, 180, 174, 167, 161, 155, 149, 142, // aa
        201, 189, 177, 211, 217, 210, 184, 196, 192, 186, 180, 173, 167, 161, 155, 149, // aa
        246, 219, 204, 228, 222, 216, 210, 204, 198, 191, 185, 179, 173, 167, 161, 155, // aa
        239, 235, 239, 233, 227, 221, 215, 209, 220, 197, 191, 185, 179, 173, 167, 161, // aa
        236, 250, 244, 238, 226, 226, 216, 234, 218, 211, 197, 191, 185, 180, 174, 168, // aa
        261, 255, 249, 243, 238, 232, 226, 220, 222, 213, 203, 197, 191, 186, 180, 174, // aa
        266, 260, 254, 249, 243, 237, 232, 226, 220, 215, 209, 203, 198, 192, 186, 180, // aa
        271, 265, 259, 254, 248, 243, 237, 231, 226, 220, 215, 209, 204, 198, 192, 187, // aa
        275, 270, 264, 259, 253, 248, 243, 237, 232, 226, 221, 215, 210, 204, 199, 193, // aa
        280, 275, 270, 264, 259, 253, 248, 243, 237, 232, 226, 221, 216, 210, 205, 200 // aa
    };
    const int16_t ve_keys_x[16] = {
        600, 720, 840, 990, 1170, 1380, 1650, 1950, 2310, 2730, 3210, 3840, 4530, 5370, 6360, 7500};
    const int16_t ve_keys_z[16] = {
        200, 254, 308, 362, 416, 470, 524, 578, 632, 686, 740, 794, 848, 902, 956, 1010};
    flipper_ecu_map_set_keys_x(set->maps[VE], ve_keys_x);
    flipper_ecu_map_set_keys_z_3d(set->maps[VE], ve_keys_z);
    flipper_ecu_map_set_values_3d(set->maps[VE], ve_values);

    // ignition angle cranking
    const int16_t ign_angle_cranking_values[16] = {
        10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10};
    const int16_t ign_angle_cranking_keys[16] = {
        200, 240, 280, 320, 360, 400, 440, 480, 520, 560, 600, 640, 680, 720, 760, 800};
    flipper_ecu_map_set_keys_x(set->maps[IGN_ANGLE_CRANKING], ign_angle_cranking_keys);
    flipper_ecu_map_set_values_2d(set->maps[IGN_ANGLE_CRANKING], ign_angle_cranking_values);

    // ignition angle idle
    const int16_t ign_angle_idle_values[16] = {
        25, 25, 25, 25, 25, 25, 27, 28, 36, 40, 43, 45, 45, 45, 45, 45};
    const int16_t ign_angle_idle_keys[16] = {
        600, 720, 840, 990, 1170, 1380, 1650, 1950, 2310, 2730, 3210, 3840, 4530, 5370, 6360, 7500};
    flipper_ecu_map_set_keys_x(set->maps[IGN_ANGLE_IDLE], ign_angle_idle_keys);
    flipper_ecu_map_set_values_2d(set->maps[IGN_ANGLE_IDLE], ign_angle_idle_values);

    // injection afterstart enrichment
    const int16_t inj_afterstart_enrichment_values[16] = {
        453, 406, 383, 352, 328, 313, 305, 289, 281, 266, 258, 242, 234, 219, 211, 203};
    const int16_t inj_afterstart_enrichment_keys[16] = {
        -30, -20, -10, 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120};
    flipper_ecu_map_set_keys_x(
        set->maps[INJ_AFTERSTART_ENRICHMENT], inj_afterstart_enrichment_keys);
    flipper_ecu_map_set_values_2d(
        set->maps[INJ_AFTERSTART_ENRICHMENT], inj_afterstart_enrichment_values);

    // injection warmup enrichment
    const int16_t inj_warmup_enrichment_values[16] = {
        534, 483, 423, 359, 282, 200, 180, 110, 50, 0, 0, 0, 0, 0, 0, 0};
    const int16_t inj_warmup_enrichment_keys[16] = {
        -30, -20, -10, 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120};
    flipper_ecu_map_set_keys_x(set->maps[INJ_WARMUP_ENRICHMENT], inj_warmup_enrichment_keys);
    flipper_ecu_map_set_values_2d(set->maps[INJ_WARMUP_ENRICHMENT], inj_warmup_enrichment_values);

    // idle rpm
    const int16_t idle_rpm_values[16] = {
        1600, 1570, 1550, 1500, 1350, 1300, 1190, 1070, 990, 800, 800, 800, 800, 800, 800, 804};
    const int16_t idle_rpm_keys[16] = {
        -30, -20, -10, 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120};
    flipper_ecu_map_set_keys_x(set->maps[IDLE_RPM], idle_rpm_keys);
    flipper_ecu_map_set_values_2d(set->maps[IDLE_RPM], idle_rpm_values);

    // maps end

    set->engine_displacement = 1200; // liters * 1000
    set->cylinders_count = 4;

    set->inj_flow = 1.9219;

    set->idle_valve_pwm_freq = 500;
    set->idle_valve_total_steps = 255;
    set->idle_closed_loop = true;

    set->idle_tps_value = 0; // %

    set->idle_valve_position_on_ignition_on = 150;

    set->cranking_end_rpm = 500;

    set->afterstart_enrichment_rotations = 160;
}
