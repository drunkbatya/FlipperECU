#include "../flipper_ecu_gui_i.h"

typedef enum {
    FlipperECUSceneSettingsSubmenuIndexAdjustments,
    FlipperECUSceneSettingsSubmenuIndexConfig,
    FlipperECUSceneSettingsSubmenuIndexLoad,
    FlipperECUSceneSettingsSubmenuIndexSave
} FlipperECUSceneSettingsSubmenuIndex;

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
    submenu_set_header(app->submenu, "Test");
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
        }
    }
    return success;
}

void flipper_ecu_scene_settings_on_exit(void* context) {
    FlipperECUGui* app = context;
    submenu_reset(app->submenu);
}
