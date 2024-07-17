#include "flipper_ecu_map.h"

#include <furi.h>

struct FlipperECUMap {
    FlipperECUMapType type; // 2D or 3D
    uint8_t map_x_size; // eg: 16 values for 2D map or first '16' in '16x16' in 3D maps.
    uint8_t map_z_size; // eg: second '16' in '16x16'. In 3D maps.  TODO: name
    int16_t value_min; // editable value min
    int16_t value_max; // editable value max
    int16_t* values; // editable values. size = map_x_size * map_z_size
    int16_t* keys; // size = map_x_size
    char name[MAP_NAME_SIZE]; // general map name
    char x_name[X_Y_NAME_SIZE]; // 'x' axis name
    char z_name[X_Y_NAME_SIZE]; // 'z' axis name. no care for 2D maps
    char values_name[X_Y_NAME_SIZE]; // 'y' axis name
};

void flipper_ecu_map_set_names(
    FlipperECUMap* map,
    const char* name,
    const char* x_name,
    const char* z_name,
    const char* values_name) {
    strncpy(map->name, name, MAP_NAME_SIZE - 1);
    strncpy(map->x_name, x_name, X_Y_NAME_SIZE - 1);
    strncpy(map->values_name, values_name, X_Y_NAME_SIZE - 1);
    if(map->type == FlipperECUMapType3D) {
        strncpy(map->z_name, z_name, X_Y_NAME_SIZE - 1);
    }
}

void flipper_ecu_map_set_ranges(FlipperECUMap* map, int16_t value_min, int16_t value_max) {
    map->value_min = value_min;
    map->value_max = value_max;
}

int16_t flipper_ecu_map_get_value_by_index(FlipperECUMap* map, uint8_t index) {
    furi_check(index < map->map_x_size);
    return map->values[index];
}

int16_t flipper_ecu_map_get_key_by_index(FlipperECUMap* map, uint8_t index) {
    furi_check(index < map->map_x_size);
    return map->keys[index];
}

const char* flipper_ecu_map_get_map_name(FlipperECUMap* map) {
    return map->name;
}

const char* flipper_ecu_map_get_x_name(FlipperECUMap* map) {
    return map->x_name;
}

const char* flipper_ecu_map_get_values_name(FlipperECUMap* map) {
    return map->values_name;
}

void flipper_ecu_map_set_value_by_index(FlipperECUMap* map, uint8_t index, int16_t value) {
    furi_check(index < map->map_x_size);
    map->values[index] = value;
}

void flipper_ecu_map_set_values(FlipperECUMap* map, const int16_t* values) {
    memcpy(map->values, values, sizeof(int16_t) * map->map_x_size);
}

void flipper_ecu_map_set_keys(FlipperECUMap* map, const int16_t* keys) {
    memcpy(map->keys, keys, sizeof(int16_t) * map->map_x_size);
}

int16_t flipper_ecu_map_get_map_x_size(FlipperECUMap* map) {
    return map->map_x_size;
}

int16_t flipper_ecu_map_get_value_min(FlipperECUMap* map) {
    return map->value_min;
}

int16_t flipper_ecu_map_get_value_max(FlipperECUMap* map) {
    return map->value_max;
}

FlipperECUMap* flipper_ecu_map_create_alloc(
    FlipperECUMapType type,
    uint8_t map_x_size,
    uint8_t map_z_size // no care for 2D maps
) {
    const uint16_t map_mem_keys_size = map_x_size * sizeof(int16_t);
    uint32_t map_mem_values_size = map_x_size * sizeof(int16_t);
    if(type == FlipperECUMapType3D) {
        // 2D map size = sizeof struct + map_x_size + headers (map_x_size)
        // 3D map size = sizeof struct + (map_x_size * map_z_size) + headers (map_x_size)
        map_mem_values_size *= map_z_size;
    }
    void* map_mem = malloc(sizeof(FlipperECUMap) + map_mem_values_size + map_mem_keys_size);
    FlipperECUMap* map = (FlipperECUMap*)map_mem;

    map->type = type;
    map->map_x_size = map_x_size;
    map->map_z_size = map_z_size;

    map->values = (map_mem + sizeof(FlipperECUMap));
    map->keys = (map_mem + sizeof(FlipperECUMap) + map_mem_values_size);

    return map;
}

void flipper_ecu_map_free(FlipperECUMap* map) {
    free(map);
}
