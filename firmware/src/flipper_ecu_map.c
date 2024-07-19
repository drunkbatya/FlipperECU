#include "flipper_ecu_map.h"

#include <furi.h>

struct FlipperECUMap {
    FlipperECUMapType type; // 2D or 3D
    char name[MAP_NAME_SIZE]; // general map name
    char x_name[X_Y_NAME_SIZE]; // 'x' axis name
    char z_name[X_Y_NAME_SIZE]; // 'z' axis name. no care for 2D maps
    char values_name[X_Y_NAME_SIZE]; // 'y' axis name
    uint8_t map_x_size; // eg: 16 values for 2D map or first '16' in '16x16' in 3D maps.
    uint8_t map_z_size; // eg: second '16' in '16x16'. In 3D maps.  TODO: name
    int16_t value_min; // editable value min
    int16_t value_max; // editable value max
    int16_t* values; // editable values. size = map_x_size * map_z_size, ** for 3D maps.
    int16_t* keys_x; // size = map_x_size
    int16_t* keys_z; // size = map_z_size
};

void flipper_ecu_map_set_names_2d(
    FlipperECUMap* map,
    const char* name,
    const char* x_name,
    const char* values_name) {
    furi_check(map->type == FlipperECUMapType2D);
    strncpy(map->name, name, MAP_NAME_SIZE - 1);
    strncpy(map->x_name, x_name, X_Y_NAME_SIZE - 1);
    strncpy(map->values_name, values_name, X_Y_NAME_SIZE - 1);
}

void flipper_ecu_map_set_names_3d(
    FlipperECUMap* map,
    const char* name,
    const char* x_name,
    const char* z_name,
    const char* values_name) {
    furi_check(map->type == FlipperECUMapType3D);
    strncpy(map->name, name, MAP_NAME_SIZE - 1);
    strncpy(map->x_name, x_name, X_Y_NAME_SIZE - 1);
    strncpy(map->values_name, values_name, X_Y_NAME_SIZE - 1);
    strncpy(map->z_name, z_name, X_Y_NAME_SIZE - 1);
}

void flipper_ecu_map_set_ranges(FlipperECUMap* map, int16_t value_min, int16_t value_max) {
    map->value_min = value_min;
    map->value_max = value_max;
}

int16_t flipper_ecu_map_get_value_by_index_2d(FlipperECUMap* map, uint8_t index) {
    furi_check(map->type == FlipperECUMapType2D);
    furi_check(index < map->map_x_size);
    return map->values[index];
}

int16_t
    flipper_ecu_map_get_value_by_index_3d(FlipperECUMap* map, uint8_t index_x, uint8_t index_z) {
    furi_check(map->type == FlipperECUMapType3D);
    furi_check(index_x < map->map_x_size);
    furi_check(index_z < map->map_z_size);
    return map->values[(index_z * map->map_x_size) + index_x];
}

int16_t flipper_ecu_map_get_key_x_by_index(FlipperECUMap* map, uint8_t index_x) {
    furi_check(index_x < map->map_x_size);
    return map->keys_x[index_x];
}

int16_t flipper_ecu_map_get_key_z_by_index_3d(FlipperECUMap* map, uint8_t index_z) {
    furi_check(map->type == FlipperECUMapType3D);
    furi_check(index_z < map->map_z_size);
    furi_check(map->keys_z);
    return map->keys_z[index_z];
}

const char* flipper_ecu_map_get_map_name(FlipperECUMap* map) {
    return map->name;
}

const char* flipper_ecu_map_get_x_name(FlipperECUMap* map) {
    return map->x_name;
}

const char* flipper_ecu_map_get_z_name_3d(FlipperECUMap* map) {
    furi_check(map->type == FlipperECUMapType3D);
    return map->z_name;
}

const char* flipper_ecu_map_get_values_name(FlipperECUMap* map) {
    return map->values_name;
}

void flipper_ecu_map_set_value_by_index_2d(FlipperECUMap* map, uint8_t index, int16_t value) {
    furi_check(map->type == FlipperECUMapType2D);
    furi_check(index < map->map_x_size);
    map->values[index] = value;
}

void flipper_ecu_map_set_value_by_index_3d(
    FlipperECUMap* map,
    uint8_t index_x,
    uint8_t index_z,
    int16_t value) {
    furi_check(map->type == FlipperECUMapType3D);
    furi_check(index_x < map->map_x_size);
    furi_check(index_z < map->map_z_size);
    map->values[(index_z * map->map_x_size) + index_x] = value;
}

void flipper_ecu_map_set_values_2d(FlipperECUMap* map, const int16_t* values) {
    furi_check(map->type == FlipperECUMapType2D);
    memcpy(map->values, values, sizeof(int16_t) * map->map_x_size);
}

void flipper_ecu_map_set_values_3d(FlipperECUMap* map, const int16_t* values) {
    furi_check(map->type == FlipperECUMapType3D);
    memcpy(map->values, values, sizeof(int16_t) * map->map_x_size * map->map_z_size);
}

void flipper_ecu_map_set_keys_x(FlipperECUMap* map, const int16_t* keys_x) {
    memcpy(map->keys_x, keys_x, sizeof(int16_t) * map->map_x_size);
}

void flipper_ecu_map_set_keys_z_3d(FlipperECUMap* map, const int16_t* keys_z) {
    furi_check(map->type == FlipperECUMapType3D);
    memcpy(map->keys_z, keys_z, sizeof(int16_t) * map->map_z_size);
}

uint8_t flipper_ecu_map_get_map_x_size(FlipperECUMap* map) {
    return map->map_x_size;
}

uint8_t flipper_ecu_map_get_map_z_size_3d(FlipperECUMap* map) {
    furi_check(map->type == FlipperECUMapType3D);
    return map->map_z_size;
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
    furi_check(map_x_size > 1);
    uint16_t map_mem_keys_size = map_x_size * sizeof(int16_t);
    uint16_t map_mem_values_size = map_x_size * sizeof(int16_t);
    uint16_t map_mem_keys_x_size = map_x_size * sizeof(int16_t);
    if(type == FlipperECUMapType3D) {
        furi_check(map_z_size > 1); // i think nobody wants a 1-layer'ed 3D map
        // 2D map size = sizeof struct + map_x_size + headers (map_x_size)
        // 3D map size = sizeof struct + (map_x_size * map_z_size) + headers (map_x_size * map_z_size)
        map_mem_values_size *= map_z_size;
        map_mem_keys_size *= map_z_size;
    }
    void* map_mem = malloc(sizeof(FlipperECUMap) + map_mem_values_size + map_mem_keys_size);
    FlipperECUMap* map = (FlipperECUMap*)map_mem;

    map->type = type;
    map->map_x_size = map_x_size;
    map->values = (map_mem + sizeof(FlipperECUMap));
    map->keys_x = (map_mem + sizeof(FlipperECUMap) + map_mem_values_size);

    if(type == FlipperECUMapType3D) {
        map->map_z_size = map_z_size;
        map->keys_z =
            (map_mem + sizeof(FlipperECUMap) + map_mem_values_size + map_mem_keys_x_size);
    } else {
        map->map_z_size = 0;
        map->keys_z = NULL;
    }

    return map;
}

void flipper_ecu_map_free(FlipperECUMap* map) {
    free(map);
}

int16_t flipper_ecu_map_interpolate_2d(FlipperECUMap* map, int16_t key_x) {
    int16_t ret = 0;
    for(uint8_t i = 0; i < map->map_x_size; i++) {
        if(key_x == map->keys_x[i]) { // exact match
            ret = map->values[i];
            break;
        }
        if(i == map->map_x_size - 1) { // use latest value if we'r reached end of the map
            ret = map->values[i];
            break;
        }
        if(i != 0) {
            if((map->keys_x[i - 1] < key_x) &&
               (map->keys_x[i] > key_x)) { // if we'r now between two points
                ret = (map->values[i] + map->values[i - 1]) / 2; // temp
                break;
            }
        } else {
            if(key_x < map->keys_x[i]) { // lower than minimum key (not value)
                ret = map->values[i];
                break;
            }
        }
    }
    return ret;
}
