#pragma once

#include <furi.h>

#define MAP_NAME_SIZE 20 + 1
#define X_Y_NAME_SIZE 11 + 1

typedef enum { FlipperECUMapType2D = 0, FlipperECUMapType3D = 0x01 } FlipperECUMapType;

typedef struct FlipperECUMap FlipperECUMap;

FlipperECUMap* flipper_ecu_map_create_alloc(
    FlipperECUMapType type,
    uint8_t map_x_size,
    uint8_t map_z_size // no care for 2D maps
);
void flipper_ecu_map_free(FlipperECUMap* map);
void flipper_ecu_map_set_names(
    FlipperECUMap* map,
    const char* name,
    const char* x_name,
    const char* z_name,
    const char* values_name);
void flipper_ecu_map_set_ranges(FlipperECUMap* map, int16_t value_min, int16_t value_max);
void flipper_ecu_map_set_keys(FlipperECUMap* map, const int16_t* keys);
void flipper_ecu_map_set_values(FlipperECUMap* map, const int16_t* values);
void flipper_ecu_map_set_value_by_index(FlipperECUMap* map, uint8_t index, int16_t value);
int16_t flipper_ecu_map_get_key_by_index(FlipperECUMap* map, uint8_t index);
int16_t flipper_ecu_map_get_value_by_index(FlipperECUMap* map, uint8_t index);
int16_t flipper_ecu_map_get_map_x_size(FlipperECUMap* map);
int16_t flipper_ecu_map_get_value_min(FlipperECUMap* map);
int16_t flipper_ecu_map_get_value_max(FlipperECUMap* map);
const char* flipper_ecu_map_get_map_name(FlipperECUMap* map);
const char* flipper_ecu_map_get_x_name(FlipperECUMap* map);
const char* flipper_ecu_map_get_values_name(FlipperECUMap* map);
int16_t flipper_ecu_map_interpolate(FlipperECUMap* map, int16_t key);
