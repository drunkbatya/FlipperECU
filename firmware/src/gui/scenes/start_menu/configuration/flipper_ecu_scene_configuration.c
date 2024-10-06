#include "../../../flipper_ecu_gui_i.h"

typedef enum {
    FlipperECUSceneConfigSubmenuIndexIgnition,
    FlipperECUSceneConfigSubmenuIndexInjection,
    FlipperECUSceneConfigSubmenuIndexIdling,
    FlipperECUSceneConfigSubmenuIndexDevices,
    FlipperECUSceneConfigSubmenuIndexSensors,
    FlipperECUSceneConfigSubmenuIndexEngineConstants,
    FlipperECUSceneConfigSubmenuIndexLoad,
    FlipperECUSceneConfigSubmenuIndexSave
} FlipperECUSceneConfigSubmenuIndex;

static void flipper_ecu_scene_config_set_header(FlipperECUGui* app) {
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

static void flipper_ecu_scene_config_submenu_callback(void* context, uint32_t index) {
    FlipperECUGui* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void flipper_ecu_scene_config_on_enter(void* context) {
    FlipperECUGui* app = context;
    submenu_add_item(
        app->submenu,
        "Ignition",
        FlipperECUSceneConfigSubmenuIndexIgnition,
        flipper_ecu_scene_config_submenu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Injection",
        FlipperECUSceneConfigSubmenuIndexInjection,
        flipper_ecu_scene_config_submenu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Idling",
        FlipperECUSceneConfigSubmenuIndexIdling,
        flipper_ecu_scene_config_submenu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Devices",
        FlipperECUSceneConfigSubmenuIndexDevices,
        flipper_ecu_scene_config_submenu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Sensors",
        FlipperECUSceneConfigSubmenuIndexSensors,
        flipper_ecu_scene_config_submenu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Engine constants",
        FlipperECUSceneConfigSubmenuIndexEngineConstants,
        flipper_ecu_scene_config_submenu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Load",
        FlipperECUSceneConfigSubmenuIndexLoad,
        flipper_ecu_scene_config_submenu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Save",
        FlipperECUSceneConfigSubmenuIndexSave,
        flipper_ecu_scene_config_submenu_callback,
        app);
    flipper_ecu_scene_config_set_header(app);
    submenu_set_selected_item(
        app->submenu, scene_manager_get_scene_state(app->scene_manager, FlipperECUSceneConfig));
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipperECUGuiViewSubmenu);
}

bool flipper_ecu_scene_config_on_event(void* context, SceneManagerEvent event) {
    FlipperECUGui* app = context;
    bool success = false;
    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(app->scene_manager, FlipperECUSceneConfig, event.event);
        if(event.event == FlipperECUSceneConfigSubmenuIndexIgnition) {
            scene_manager_next_scene(app->scene_manager, FlipperECUSceneConfigIgnition);
            success = true;
        } else if(event.event == FlipperECUSceneConfigSubmenuIndexInjection) {
            scene_manager_next_scene(app->scene_manager, FlipperECUSceneConfigInjection);
            success = true;
        } else if(event.event == FlipperECUSceneConfigSubmenuIndexIdling) {
            scene_manager_next_scene(app->scene_manager, FlipperECUSceneConfigIdling);
            success = true;
        } else if(event.event == FlipperECUSceneConfigSubmenuIndexDevices) {
            //scene_manager_next_scene(app->scene_manager, FlipperECUSceneMapEditor);
            success = true;
        } else if(event.event == FlipperECUSceneConfigSubmenuIndexSensors) {
            scene_manager_next_scene(app->scene_manager, FlipperECUSceneConfigSensors);
            success = true;
        } else if(event.event == FlipperECUSceneConfigSubmenuIndexEngineConstants) {
            //scene_manager_next_scene(app->scene_manager, FlipperECUSceneIgnition);
            success = true;
        } else if(event.event == FlipperECUSceneConfigSubmenuIndexLoad) {
            scene_manager_next_scene(app->scene_manager, FlipperECUSceneLoad);
            success = true;
        } else if(event.event == FlipperECUSceneConfigSubmenuIndexSave) {
            scene_manager_next_scene(app->scene_manager, FlipperECUSceneSave);
            success = true;
        }
    }
    return success;
}

void flipper_ecu_scene_config_on_exit(void* context) {
    FlipperECUGui* app = context;
    submenu_reset(app->submenu);
}
