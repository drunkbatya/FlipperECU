#include "../flipper_ecu_gui_i.h"

typedef enum {
    FlipperECUSceneSettingsSubmenuIndexAdjustments,
    FlipperECUSceneSettingsSubmenuIndexConfig,
    FlipperECUSceneSettingsSubmenuIndexLoad,
    FlipperECUSceneSettingsSubmenuIndexSave
} FlipperECUSceneSettingsSubmenuIndex;

static void flipper_ecu_scene_setting_set_header(FlipperECUGui* app) {
    FuriString* temp_str = furi_string_alloc();
    if(!furi_string_end_with(app->file_path, ENGINE_SETTINGS_FILE_EXT)) {
        furi_string_set_str(temp_str, "No file loaded");
    } else {
        furi_string_set(temp_str, app->file_path);
        size_t file_name_start = furi_string_search_rchar(temp_str, '/');
        furi_string_right(temp_str, file_name_start + 1);
        size_t file_name_ext = furi_string_search_str(temp_str, ENGINE_SETTINGS_FILE_EXT);
        furi_string_left(temp_str, file_name_ext);
    }
    submenu_set_header(app->submenu, furi_string_get_cstr(temp_str));
    furi_string_free(temp_str);
}

static void flipper_ecu_scene_settings_submenu_callback(void* context, uint32_t index) {
    FlipperECUGui* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void flipper_ecu_scene_settings_on_enter(void* context) {
    FlipperECUGui* app = context;
    submenu_add_item(
        app->submenu,
        "Adjustments",
        FlipperECUSceneSettingsSubmenuIndexAdjustments,
        flipper_ecu_scene_settings_submenu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Configuration",
        FlipperECUSceneSettingsSubmenuIndexConfig,
        flipper_ecu_scene_settings_submenu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Load",
        FlipperECUSceneSettingsSubmenuIndexLoad,
        flipper_ecu_scene_settings_submenu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Save",
        FlipperECUSceneSettingsSubmenuIndexSave,
        flipper_ecu_scene_settings_submenu_callback,
        app);
    flipper_ecu_scene_setting_set_header(app);
    submenu_set_selected_item(
        app->submenu, scene_manager_get_scene_state(app->scene_manager, FlipperECUSceneSettings));
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipperECUGuiViewSubmenu);
}

bool flipper_ecu_scene_settings_on_event(void* context, SceneManagerEvent event) {
    FlipperECUGui* app = context;
    bool success = false;
    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(app->scene_manager, FlipperECUSceneSettings, event.event);
        if(event.event == FlipperECUSceneSettingsSubmenuIndexConfig) {
            scene_manager_next_scene(app->scene_manager, FlipperECUSceneConfig);
            success = true;
        } else if(event.event == FlipperECUSceneSettingsSubmenuIndexAdjustments) {
            scene_manager_next_scene(app->scene_manager, FlipperECUSceneMapEditor);
            success = true;
        } else if(event.event == FlipperECUSceneSettingsSubmenuIndexLoad) {
            scene_manager_next_scene(app->scene_manager, FlipperECUSceneLoad);
            success = true;
        } else if(event.event == FlipperECUSceneSettingsSubmenuIndexSave) {
            scene_manager_next_scene(app->scene_manager, FlipperECUSceneSave);
            success = true;
        }
    }
    return success;
}

void flipper_ecu_scene_settings_on_exit(void* context) {
    FlipperECUGui* app = context;
    submenu_reset(app->submenu);
}
