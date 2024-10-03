#pragma once

#include <furi.h>
#include <storage/storage.h>

#define MAP_NAME_SIZE 20 + 1
#define X_Y_NAME_SIZE 11 + 1

typedef enum { FlipperECUMapType2D = 0, FlipperECUMapType3D = 0x01 } FlipperECUMapType;

typedef struct FlipperECUMap FlipperECUMap;

FlipperECUMap* flipper_ecu_map_create_alloc_2d(uint16_t map_x_size);
FlipperECUMap* flipper_ecu_map_create_alloc_3d(uint16_t map_x_size, uint16_t map_z_size);
void flipper_ecu_map_free(FlipperECUMap* map);
void flipper_ecu_map_set_names_2d(
    FlipperECUMap* map,
    const char* name,
    const char* x_name,
    const char* values_name);
void flipper_ecu_map_set_names_3d(
    FlipperECUMap* map,
    const char* name,
    const char* x_name,
    const char* z_name,
    const char* values_name);
void flipper_ecu_map_set_ranges(FlipperECUMap* map, int16_t value_min, int16_t value_max);
void flipper_ecu_map_set_keys_x(FlipperECUMap* map, const int16_t* keys_x);
void flipper_ecu_map_set_keys_z_3d(FlipperECUMap* map, const int16_t* keys_z);
void flipper_ecu_map_set_values_2d(FlipperECUMap* map, const int16_t* values);
void flipper_ecu_map_set_values_3d(FlipperECUMap* map, const int16_t* values);
void flipper_ecu_map_set_value_by_index_2d(FlipperECUMap* map, uint16_t index, int16_t value);
void flipper_ecu_map_set_value_by_index_3d(
    FlipperECUMap* map,
    uint16_t index_x,
    uint16_t index_z,
    int16_t value);
int16_t flipper_ecu_map_get_value_by_index_2d(FlipperECUMap* map, uint16_t index);
int16_t
    flipper_ecu_map_get_value_by_index_3d(FlipperECUMap* map, uint16_t index_x, uint16_t index_z);
int16_t flipper_ecu_map_get_key_x_by_index(FlipperECUMap* map, uint16_t index_x);
int16_t flipper_ecu_map_get_key_z_by_index_3d(FlipperECUMap* map, uint16_t index_z);
uint16_t flipper_ecu_map_get_map_x_size(FlipperECUMap* map);
uint16_t flipper_ecu_map_get_map_z_size_3d(FlipperECUMap* map);
int16_t flipper_ecu_map_get_value_min(FlipperECUMap* map);
int16_t flipper_ecu_map_get_value_max(FlipperECUMap* map);
const char* flipper_ecu_map_get_map_name(FlipperECUMap* map);
const char* flipper_ecu_map_get_x_name(FlipperECUMap* map);
const char* flipper_ecu_map_get_z_name_3d(FlipperECUMap* map);
const char* flipper_ecu_map_get_values_name(FlipperECUMap* map);
FlipperECUMapType flipper_ecu_map_get_map_type(FlipperECUMap* map);
int16_t flipper_ecu_map_interpolate_2d(const FlipperECUMap* map, int16_t key_x);
int16_t flipper_ecu_map_interpolate_3d(const FlipperECUMap* map, int16_t key_x, int16_t key_z);
bool flipper_ecu_map_load(FlipperECUMap* map, File* file);
bool flipper_ecu_map_save(FlipperECUMap* map, File* file);
