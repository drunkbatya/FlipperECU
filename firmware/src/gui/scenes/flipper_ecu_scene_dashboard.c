#include "../flipper_ecu_gui_i.h"

void flipper_ecu_scene_dashboard_on_enter(void* context) {
    FlipperECUGui* app = context;
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipperECUGuiViewDashboard);
}

bool flipper_ecu_scene_dashboard_on_event(void* context, SceneManagerEvent event) {
    FlipperECUGui* app = context;
    if(event.type == SceneManagerEventTypeTick) {
        flipper_ecu_view_dashboard_view_update(app->view_dashboard);
        return true;
    }
    return false;
}

void flipper_ecu_scene_dashboard_on_exit(void* context) {
    FlipperECUGui* app = context;
    UNUSED(app);
}
