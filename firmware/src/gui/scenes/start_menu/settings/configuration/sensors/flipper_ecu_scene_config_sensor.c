#include "../../../../../flipper_ecu_gui_i.h"

typedef enum {
    FlipperECUSceneConfigSensorsSubmenuIndexCKPS,
    FlipperECUSceneConfigSensorsSubmenuIndexMAP,
    FlipperECUSceneConfigSensorsSubmenuIndexTPS,
} FlipperECUSceneConfigSensorsSubmenuIndex;

static void flipper_ecu_scene_config_sensor_submenu_callback(void* context, uint32_t index) {
    FlipperECUGui* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void flipper_ecu_scene_config_sensor_on_enter(void* context) {
    FlipperECUGui* app = context;
    submenu_add_item(
        app->submenu,
        "CKPS",
        FlipperECUSceneConfigSensorsSubmenuIndexCKPS,
        flipper_ecu_scene_config_sensor_submenu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "MAP",
        FlipperECUSceneConfigSensorsSubmenuIndexMAP,
        flipper_ecu_scene_config_sensor_submenu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "TPS",
        FlipperECUSceneConfigSensorsSubmenuIndexTPS,
        flipper_ecu_scene_config_sensor_submenu_callback,
        app);
    submenu_set_selected_item(
        app->submenu,
        scene_manager_get_scene_state(app->scene_manager, FlipperECUSceneConfigSensor));
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipperECUGuiViewSubmenu);
}

bool flipper_ecu_scene_config_sensor_on_event(void* context, SceneManagerEvent event) {
    FlipperECUGui* app = context;
    bool success = false;
    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(
            app->scene_manager, FlipperECUSceneConfigSensor, event.event);
        if(event.event == FlipperECUSceneConfigSensorsSubmenuIndexCKPS) {
            scene_manager_next_scene(app->scene_manager, FlipperECUSceneConfigCKPS);
            success = true;
        }
        //if(event.event == FlipperECUSceneConfigSensorsSubmenuIndexMAP) {
        //    scene_manager_next_scene(app->scene_manager, FlipperECUSceneConfigMAP);
        //    success = true;
        //}
        if(event.event == FlipperECUSceneConfigSensorsSubmenuIndexTPS) {
            scene_manager_next_scene(app->scene_manager, FlipperECUSceneConfigTPS);
            success = true;
        }
    }
    return success;
}

void flipper_ecu_scene_config_sensor_on_exit(void* context) {
    FlipperECUGui* app = context;
    submenu_reset(app->submenu);
}
