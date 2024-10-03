#include "../../../../../../flipper_ecu_gui_i.h"

void flipper_ecu_scene_config_ignition_idle_map_on_enter(void* context) {
    FlipperECUGui* app = context;
    flipper_ecu_view_map_editor_load_map(
        app->view_map_editor, app->ecu_app->engine_settings->maps[IGN_ANGLE_IDLE]);
}

bool flipper_ecu_scene_config_ignition_idle_map_on_event(void* context, SceneManagerEvent event) {
    FlipperECUGui* app = context;
    UNUSED(app);
    UNUSED(event);
    bool success = false;
    return success;
}

void flipper_ecu_scene_config_ignition_idle_map_on_exit(void* context) {
    FlipperECUGui* app = context;
    UNUSED(app);
}
