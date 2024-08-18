#include "flipper_ecu_files.h"

#include "gui/flipper_ecu_gui_i.h"

static bool flipper_ecu_files_save_maps(FlipperECUApp* app, File* file) {
    for(uint8_t map_index = 0; map_index < IGN_MAP_COUNT; map_index++) {
        FlipperECUMap* map = app->engine_settings->maps[map_index];
        if(!flipper_ecu_map_save(map, file)) return false;
    }
    return true;
}

static bool flipper_ecu_files_load_maps(FlipperECUApp* app, File* file) {
    for(uint8_t map_index = 0; map_index < IGN_MAP_COUNT; map_index++) {
        FlipperECUMap* map = app->engine_settings->maps[map_index];
        if(!flipper_ecu_map_load(map, file)) return false;
    }
    return true;
}

bool flipper_ecu_files_save(FlipperECUApp* app) {
    bool success = false;
    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);
    do {
        if(furi_string_end_with(app->gui->file_path, ENGINE_SETTINGS_FILE_EXT)) {
            if(!storage_simply_remove(storage, furi_string_get_cstr(app->gui->file_path))) break;
            size_t filename_start = furi_string_search_rchar(app->gui->file_path, '/');
            furi_string_left(app->gui->file_path, filename_start);
        }
        furi_string_cat_printf(
            app->gui->file_path, "/%s%s", app->gui->text_buffer, ENGINE_SETTINGS_FILE_EXT);
        if(!storage_file_open(
               file, furi_string_get_cstr(app->gui->file_path), FSAM_WRITE, FSOM_CREATE_NEW))
            break;
        if(!flipper_ecu_files_save_maps(app, file)) break;

        success = true;
    } while(false);

    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
    return success;
}

bool flipper_ecu_files_load(FlipperECUApp* app) {
    bool success = false;
    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);
    flipper_ecu_sync_worker_send_stop(app->sync_worker);
    flipper_ecu_sync_worker_await_stop(app->sync_worker);
    do {
        if(!storage_file_open(
               file, furi_string_get_cstr(app->gui->file_path), FSAM_READ, FSOM_OPEN_EXISTING))
            break;
        if(!flipper_ecu_files_load_maps(app, file)) break;
        success = true;
    } while(false);

    flipper_ecu_sync_worker_start(app->sync_worker);

    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
    return success;
}
