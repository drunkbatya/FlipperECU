#include "../../../../../../flipper_ecu_gui_i.h"

void flipper_ecu_scene_config_injection_ve_map_grid_on_enter(void* context) {
    FlipperECUGui* app = context;
    flipper_ecu_view_map_editor_3d_grid_load_map(
        app->view_map_editor_3d_grid, app->ecu_app->engine_settings->maps[VE]);
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipperECUGuiViewMapEditor3DGrid);
}

bool flipper_ecu_scene_config_injection_ve_map_grid_on_event(
    void* context,
    SceneManagerEvent event) {
    FlipperECUGui* app = context;
    UNUSED(app);
    if(event.type == SceneManagerEventTypeTick) {
        flipper_ecu_view_map_editor_3d_grid_view_update(app->view_map_editor_3d_grid);
        return true;
    }
    return false;
}

void flipper_ecu_scene_config_injection_ve_map_grid_on_exit(void* context) {
    FlipperECUGui* app = context;
    flipper_ecu_view_map_editor_3d_grid_reset(app->view_map_editor_3d_grid);
}
