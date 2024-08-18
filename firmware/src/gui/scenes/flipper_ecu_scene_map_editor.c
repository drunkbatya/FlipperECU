#include "../flipper_ecu_gui_i.h"

void flipper_ecu_scene_map_editor_on_enter(void* context) {
    FlipperECUGui* app = context;
    flipper_ecu_view_map_editor_load_map(
        app->view_map_editor, app->ecu_app->engine_settings->maps[IGN_MAP]);
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipperECUGuiViewMapEditor);
}

bool flipper_ecu_scene_map_editor_on_event(void* context, SceneManagerEvent event) {
    FlipperECUGui* app = context;
    UNUSED(app);
    if(event.type == SceneManagerEventTypeTick) {
        //flipper_ecu_scene_map_editor_update_view(app);
        return true;
    }
    return false;
}

void flipper_ecu_scene_map_editor_on_exit(void* context) {
    FlipperECUGui* app = context;
    UNUSED(app);
}
