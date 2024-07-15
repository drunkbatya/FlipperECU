#include "../flipper_ecu_gui_i.h"
#include "../../worker/flipper_ecu_sync_worker.h"

static void flipper_ecu_scene_dashboard_update_view(FlipperECUGui* app) {
    uint32_t rpm = flipper_ecu_sync_worker_get_rpm(app->ecu_app->sync_worker);
    flipper_ecu_view_dashboard_set_rpm(app->view_dashboard, rpm);
}

void flipper_ecu_scene_dashboard_on_enter(void* context) {
    FlipperECUGui* app = context;
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipperECUGuiViewDashboard);
}

bool flipper_ecu_scene_dashboard_on_event(void* context, SceneManagerEvent event) {
    FlipperECUGui* app = context;
    if(event.type == SceneManagerEventTypeTick) {
        flipper_ecu_scene_dashboard_update_view(app);
        return true;
    }
    return false;
}

void flipper_ecu_scene_dashboard_on_exit(void* context) {
    FlipperECUGui* app = context;
    UNUSED(app);
}
