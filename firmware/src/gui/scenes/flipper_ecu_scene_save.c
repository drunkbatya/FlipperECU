#include "../flipper_ecu_gui_i.h"
#include "../../flipper_ecu_engine_settings.h"
#include "../../flipper_ecu_files.h"
#include <toolbox/name_generator.h>

static void flipper_ecu_scene_save_set_opened_file_name(FlipperECUGui* app) {
    if(!furi_string_end_with(app->file_path, ENGINE_SETTINGS_FILE_EXT)) {
        name_generator_make_auto(app->text_buffer, FLIPPER_ECU_TEXT_BUFFER_SIZE - 1, "Test");
    } else {
        FuriString* temp_str = furi_string_alloc();
        furi_string_set(temp_str, app->file_path);
        size_t file_name_start = furi_string_search_rchar(temp_str, '/');
        furi_string_right(temp_str, file_name_start + 1);
        size_t file_name_ext = furi_string_search_str(temp_str, ENGINE_SETTINGS_FILE_EXT);
        furi_string_left(temp_str, file_name_ext);
        strncpy(
            app->text_buffer, furi_string_get_cstr(temp_str), FLIPPER_ECU_TEXT_BUFFER_SIZE - 1);
        furi_string_free(temp_str);
    }
}

static void flipper_ecu_scene_save_filename_view_result_callback(void* context) {
    FlipperECUGui* app = context;
    view_dispatcher_send_custom_event(
        app->view_dispatcher, FlipperECUGuiCustomEventTextEditResult);
}

void flipper_ecu_scene_save_on_enter(void* context) {
    FlipperECUGui* app = context;
    flipper_ecu_scene_save_set_opened_file_name(app);
    text_input_set_header_text(app->text_input, "Name the config");
    text_input_set_result_callback(
        app->text_input,
        flipper_ecu_scene_save_filename_view_result_callback,
        app,
        app->text_buffer,
        FLIPPER_ECU_TEXT_BUFFER_SIZE - 1,
        true);
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipperECUGuiViewTextInput);
}

bool flipper_ecu_scene_save_on_event(void* context, SceneManagerEvent event) {
    FlipperECUGui* app = context;
    bool success = false;
    if(event.type == SceneManagerEventTypeCustom) {
        success = true;
        if(event.event == FlipperECUGuiCustomEventTextEditResult) {
            if(flipper_ecu_files_save(app->ecu_app)) {
                scene_manager_next_scene(app->scene_manager, FlipperECUSceneSuccess);
            } else {
                dialog_message_show_storage_error(app->dialogs, "Cannot save\nfile");
                scene_manager_previous_scene(app->scene_manager);
            }
        }
    }
    return success;
}

void flipper_ecu_scene_save_on_exit(void* context) {
    UNUSED(context);
}
