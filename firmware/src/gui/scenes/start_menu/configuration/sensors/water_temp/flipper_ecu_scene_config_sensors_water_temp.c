#include "../../../../../flipper_ecu_gui_i.h"

typedef enum {
    FlipperECUSceneConfigConfigSensorsWaterTempSubmenuIndexMap,
} FlipperECUSceneConfigConfigSensorsWaterTempSubmenuIndex;

static void
    flipper_ecu_scene_config_sensors_water_temp_submenu_callback(void* context, uint32_t index) {
    FlipperECUGui* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void flipper_ecu_scene_config_sensors_water_temp_on_enter(void* context) {
    FlipperECUGui* app = context;
    submenu_add_item(
        app->submenu,
        "Map [2D]",
        FlipperECUSceneConfigConfigSensorsWaterTempSubmenuIndexMap,
        flipper_ecu_scene_config_sensors_water_temp_submenu_callback,
        app);
    submenu_set_selected_item(
        app->submenu,
        scene_manager_get_scene_state(app->scene_manager, FlipperECUSceneConfigSensorsWaterTemp));
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipperECUGuiViewSubmenu);
}

bool flipper_ecu_scene_config_sensors_water_temp_on_event(void* context, SceneManagerEvent event) {
    FlipperECUGui* app = context;
    bool success = false;
    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(
            app->scene_manager, FlipperECUSceneConfigSensorsWaterTemp, event.event);
        if(event.event == FlipperECUSceneConfigConfigSensorsWaterTempSubmenuIndexMap) {
            scene_manager_next_scene(app->scene_manager, FlipperECUSceneConfigSensorsWaterTempMap);
            success = true;
        }
    }
    return success;
}

void flipper_ecu_scene_config_sensors_water_temp_on_exit(void* context) {
    FlipperECUGui* app = context;
    submenu_reset(app->submenu);
}
