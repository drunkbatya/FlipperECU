#include "flipper_ecu_map.h"

#include <furi.h>

struct FlipperECUMap {
    FlipperECUMapType type; // 2D or 3D
    char name[MAP_NAME_SIZE]; // general map name
    char x_name[X_Y_NAME_SIZE]; // 'x' axis name
    char z_name[X_Y_NAME_SIZE]; // 'z' axis name. no care for 2D maps
    char values_name[X_Y_NAME_SIZE]; // 'y' axis name
    uint16_t map_x_size; // eg: 16 values for 2D map or first '16' in '16x16' in 3D maps.
    uint16_t map_z_size; // eg: second '16' in '16x16'. In 3D maps.  TODO: name
    int16_t value_min; // editable value min
    int16_t value_max; // editable value max
    int16_t last_used_key_x1; // to visualize currently used cells, key_x1
    int16_t last_used_key_x2; // to visualize currently used cells, key_x2
    int16_t last_used_key_z1; // to visualize currently used cells, key_z1
    int16_t last_used_key_z2; // to visualize currently used cells, key_z2
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

int16_t flipper_ecu_map_get_value_by_index_2d(FlipperECUMap* map, uint16_t index) {
    furi_check(map->type == FlipperECUMapType2D);
    furi_check(index < map->map_x_size);
    return map->values[index];
}

int16_t
    flipper_ecu_map_get_value_by_index_3d(FlipperECUMap* map, uint16_t index_x, uint16_t index_z) {
    furi_check(map->type == FlipperECUMapType3D);
    furi_check(index_x < map->map_x_size);
    furi_check(index_z < map->map_z_size);
    return map->values[(index_z * map->map_x_size) + index_x];
}

int16_t flipper_ecu_map_get_last_used_key_x1(FlipperECUMap* map) {
    return map->last_used_key_x1;
}

int16_t flipper_ecu_map_get_last_used_key_x2(FlipperECUMap* map) {
    return map->last_used_key_x2;
}

int16_t flipper_ecu_map_get_last_used_key_z1_3d(FlipperECUMap* map) {
    return map->last_used_key_z1;
}

int16_t flipper_ecu_map_get_last_used_key_z2_3d(FlipperECUMap* map) {
    return map->last_used_key_z2;
}

int16_t flipper_ecu_map_get_key_x_by_index(FlipperECUMap* map, uint16_t index_x) {
    furi_check(index_x < map->map_x_size);
    return map->keys_x[index_x];
}

int16_t flipper_ecu_map_get_key_z_by_index_3d(FlipperECUMap* map, uint16_t index_z) {
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

void flipper_ecu_map_set_value_by_index_2d(FlipperECUMap* map, uint16_t index, int16_t value) {
    furi_check(map->type == FlipperECUMapType2D);
    furi_check(index < map->map_x_size);
    map->values[index] = value;
}

void flipper_ecu_map_set_value_by_index_3d(
    FlipperECUMap* map,
    uint16_t index_x,
    uint16_t index_z,
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

uint16_t flipper_ecu_map_get_map_x_size(FlipperECUMap* map) {
    return map->map_x_size;
}

uint16_t flipper_ecu_map_get_map_z_size_3d(FlipperECUMap* map) {
    furi_check(map->type == FlipperECUMapType3D);
    return map->map_z_size;
}

int16_t flipper_ecu_map_get_value_min(FlipperECUMap* map) {
    return map->value_min;
}

int16_t flipper_ecu_map_get_value_max(FlipperECUMap* map) {
    return map->value_max;
}

FlipperECUMap* flipper_ecu_map_create_alloc_2d(uint16_t map_x_size) {
    furi_check(map_x_size > 1);
    const uint16_t map_mem_keys_size = map_x_size * sizeof(int16_t);
    const uint16_t map_mem_values_size = map_x_size * sizeof(int16_t);

    void* map_mem = malloc(sizeof(FlipperECUMap) + map_mem_values_size + map_mem_keys_size);
    FlipperECUMap* map = (FlipperECUMap*)map_mem;

    map->type = FlipperECUMapType2D;
    map->map_x_size = map_x_size;
    map->values = (map_mem + sizeof(FlipperECUMap));
    map->keys_x = (map_mem + sizeof(FlipperECUMap) + map_mem_values_size);
    map->map_z_size = 0;
    map->keys_z = NULL;

    return map;
}

FlipperECUMap* flipper_ecu_map_create_alloc_3d(uint16_t map_x_size, uint16_t map_z_size) {
    furi_check(map_x_size > 1);
    furi_check(map_z_size > 1); // i think nobody wants a 1-layer'ed 3D map
    // 2D map size = sizeof struct + map_x_size + headers (map_x_size)
    // 3D map size = sizeof struct + (map_x_size * map_z_size) + headers (map_x_size * map_z_size) + keys_z (map_z_size)
    const uint16_t map_mem_keys_x_size = map_x_size * sizeof(int16_t);
    const uint16_t map_mem_keys_z_size = map_z_size * sizeof(int16_t);
    const uint16_t map_mem_values_size = map_x_size * map_z_size * sizeof(int16_t);

    void* map_mem = malloc(
        sizeof(FlipperECUMap) + map_mem_values_size + map_mem_keys_x_size + map_mem_keys_z_size);
    FlipperECUMap* map = (FlipperECUMap*)map_mem;

    map->type = FlipperECUMapType3D;
    map->map_x_size = map_x_size;
    map->values = (map_mem + sizeof(FlipperECUMap));
    map->keys_x = (map_mem + sizeof(FlipperECUMap) + map_mem_values_size);

    map->map_z_size = map_z_size;
    map->keys_z = (map_mem + sizeof(FlipperECUMap) + map_mem_values_size + map_mem_keys_x_size);

    return map;
}

static void* flipper_ecu_map_get_map_data_mem(FlipperECUMap* map) {
    return (void*)map->values;
}

// only data size, not whole struct
static uint32_t flipper_ecu_map_get_data_mem_size(FlipperECUMap* map) {
    uint16_t map_mem_keys_x_size = map->map_x_size * sizeof(int16_t);
    uint16_t map_mem_values_size = map->map_x_size * sizeof(int16_t);

    uint32_t total_mem = 0;
    if(map->type == FlipperECUMapType3D) {
        map_mem_values_size *= map->map_z_size;
        const uint16_t map_mem_keys_z_size = map->map_z_size * sizeof(int16_t);
        total_mem += map_mem_keys_z_size;
    }
    total_mem += map_mem_values_size + map_mem_keys_x_size;
    return total_mem;
}

void flipper_ecu_map_free(FlipperECUMap* map) {
    free(map);
}

static int16_t interpolate(
    int16_t key_high,
    int16_t value_high,
    int16_t key_low,
    int16_t value_low,
    int16_t key) {
    uint16_t dx = key_high - key_low;
    int16_t dy = value_high - value_low;
    return value_low + (key - key_low) * dy / dx;
}

int16_t flipper_ecu_map_interpolate_2d(const FlipperECUMap* map, int16_t key_x) {
    if(key_x < map->keys_x[0]) {
        return map->values[0];
    }
    if(key_x > map->keys_x[map->map_x_size - 1]) {
        return map->values[map->map_x_size - 1];
    }
    uint16_t i = 0;
    for(; i < map->map_x_size - 1; i++) {
        if(map->keys_x[i + 1] > key_x) {
            break;
        }
    }

    return interpolate(
        map->keys_x[i + 1], map->values[i + 1], map->keys_x[i], map->values[i], key_x);
}

static int16_t flipper_ecu_map_interpolate_3d_sorry_this_is_temp(
    FlipperECUMap* map,
    int16_t key_x,
    uint16_t index_z) {
    if(key_x < map->keys_x[0]) {
        map->last_used_key_x1 = map->keys_x[0];
        map->last_used_key_x2 = 0;
        return map->values[(index_z * map->map_x_size) + (0)];
    }
    if(key_x > map->keys_x[map->map_x_size - 1]) {
        map->last_used_key_x1 = map->keys_x[map->map_x_size - 1];
        map->last_used_key_x2 = 0;
        return map->values[(index_z * map->map_x_size) + (map->map_x_size - 1)];
    }
    uint16_t i = 0;
    for(; i < map->map_x_size - 1; i++) {
        if(map->keys_x[i + 1] > key_x) {
            break;
        }
    }

    map->last_used_key_x1 = map->keys_x[i];
    map->last_used_key_x2 = map->keys_x[i + 1];

    return interpolate(
        map->keys_x[i + 1],
        map->values[(index_z * map->map_x_size) + i + 1],
        map->keys_x[i],
        map->values[(index_z * map->map_x_size) + i],
        key_x);
}

int16_t flipper_ecu_map_interpolate_3d(FlipperECUMap* map, int16_t key_x, int16_t key_z) {
    int16_t z_index_1 = 0;
    int16_t z_index_2 = 0;
    uint16_t index_z = 0;
    //  finding two z dots
    if(key_z <= map->keys_z[0]) { // lower than minimum z key, only one z dot to interpolate
        z_index_1 = 0;
    }
    if(key_z >=
       map->keys_z[map->map_z_size - 1]) { // bigger than maximum z key, only one z dot to interpolate
        z_index_1 = map->map_z_size - 1;
    }
    for(; index_z < map->map_z_size; index_z++) {
        if(index_z ==
           map->map_z_size - 1) { // we'r reached last point, only one z dot to interpolate
            z_index_1 = index_z;
            break;
        }
        if(map->keys_z[index_z + 1] > key_z) {
            z_index_1 = index_z;
            z_index_2 = index_z + 1;
            break;
        }
    }
    if(z_index_2 != 0) {
        int16_t value_low =
            flipper_ecu_map_interpolate_3d_sorry_this_is_temp(map, key_x, z_index_1);
        int16_t value_high =
            flipper_ecu_map_interpolate_3d_sorry_this_is_temp(map, key_x, z_index_2);
        map->last_used_key_z1 = flipper_ecu_map_get_key_z_by_index_3d(map, z_index_1);
        map->last_used_key_z2 = flipper_ecu_map_get_key_z_by_index_3d(map, z_index_2);
        return interpolate(
            map->keys_z[index_z + 1], value_high, map->keys_z[index_z], value_low, key_z);
    } else {
        map->last_used_key_z1 = flipper_ecu_map_get_key_z_by_index_3d(map, z_index_1);
        map->last_used_key_z2 = 0;
        return flipper_ecu_map_interpolate_3d_sorry_this_is_temp(map, key_x, z_index_1);
    }
}

FlipperECUMapType flipper_ecu_map_get_map_type(FlipperECUMap* map) {
    return map->type;
}

bool flipper_ecu_map_load(FlipperECUMap* map, File* file) {
    const uint32_t size = flipper_ecu_map_get_data_mem_size(map);
    return (storage_file_read(file, flipper_ecu_map_get_map_data_mem(map), size) == size);
}
bool flipper_ecu_map_save(FlipperECUMap* map, File* file) {
    const uint32_t size = flipper_ecu_map_get_data_mem_size(map);
    return (storage_file_write(file, flipper_ecu_map_get_map_data_mem(map), size) == size);
}

void flipper_ecu_map_print_to_logs_3d(FlipperECUMap* map) {
    FuriString* fstr = furi_string_alloc();
    furi_string_printf(fstr, "Map name: '%s'\r\n", map->name);
    furi_string_cat_printf(fstr, "Map data:\r\n");
    for(uint32_t vertical_i = 0; vertical_i < map->map_z_size; vertical_i++) { // rows
        for(uint32_t horizontal_i = 0; horizontal_i < map->map_x_size; horizontal_i++) { // columns
            furi_string_cat_printf(
                fstr, "%d,", map->values[(vertical_i * map->map_x_size) + horizontal_i]);
        }
        furi_string_cat_printf(fstr, "\r\n");
    }
    FURI_LOG_I("Map dump", furi_string_get_cstr(fstr));
    furi_string_free(fstr);
}
