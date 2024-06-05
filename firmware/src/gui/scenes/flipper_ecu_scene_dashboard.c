#include "../flipper_ecu_gui_i.h"
#include "../../workers/flipper_ecu_sync_worker.h"

void flipper_ecu_scene_dashboard_on_enter(void* context) {
    FlipperECUGui* app = context;
    FuriString* tmp_string = furi_string_alloc();
    furi_string_printf(
        tmp_string, "RPM: %lu", flipper_ecu_sync_worker_get_rpm(app->ecu_app->sync_worker));
    widget_add_string_element(
        app->widget, 0, 0, AlignLeft, AlignTop, FontSecondary, furi_string_get_cstr(tmp_string));
    furi_string_free(tmp_string);
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipperECUGuiViewWidget);
}

bool flipper_ecu_scene_dashboard_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void flipper_ecu_scene_dashboard_on_exit(void* context) {
    FlipperECUGui* app = context;
    widget_reset(app->widget);
}
