#include "flipper_ecu_gui_i.h"

static bool flipper_ecu_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    FlipperECUGui* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool flipper_ecu_back_event_callback(void* context) {
    furi_assert(context);
    FlipperECUGui* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static FlipperECUGui* flipper_ecu_gui_alloc(void) {
    FlipperECUGui* app = malloc(sizeof(FlipperECUGui));

    app->gui = furi_record_open(RECORD_GUI);
    app->scene_manager = scene_manager_alloc(&flipper_ecu_scene_handlers, app);
    app->view_dispatcher = view_dispatcher_alloc();
    app->submenu = submenu_alloc();
    app->widget = widget_alloc();

    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, flipper_ecu_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, flipper_ecu_back_event_callback);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_add_view(
        app->view_dispatcher, FlipperECUGuiViewSubmenu, submenu_get_view(app->submenu));
    view_dispatcher_add_view(
        app->view_dispatcher, FlipperECUGuiViewWidget, widget_get_view(app->widget));
    scene_manager_next_scene(app->scene_manager, FlipperECUSceneStart);

    return app;
}

static void flipper_ecu_gui_free(FlipperECUGui* app) {
    view_dispatcher_remove_view(app->view_dispatcher, FlipperECUGuiViewSubmenu);
    view_dispatcher_remove_view(app->view_dispatcher, FlipperECUGuiViewWidget);
    widget_free(app->widget);
    submenu_free(app->submenu);
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);
    furi_record_close(RECORD_GUI);
    free(app);
}

int32_t flipper_ecu_gui_thread(void* arg) {
    FlipperECUGui* app = flipper_ecu_gui_alloc();
    app->ecu_app = arg;
    view_dispatcher_run(app->view_dispatcher);
    flipper_ecu_gui_free(app);
    return 0;
}
