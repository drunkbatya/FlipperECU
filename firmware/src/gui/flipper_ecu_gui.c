#include "flipper_ecu_gui_i.h"

#define TAG "FlipperECUGui"
#define FLIPPER_ECU_GUI_TICK 250

static bool flipper_ecu_gui_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    FlipperECUGui* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool flipper_ecu_gui_back_event_callback(void* context) {
    furi_assert(context);
    FlipperECUGui* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void flipper_ecu_gui_tick_event_callback(void* context) {
    furi_assert(context);
    FlipperECUGui* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

static int32_t flipper_ecu_gui_thread(void* arg) {
    FlipperECUGui* app = arg;
    FURI_LOG_I(TAG, "thread started");
    view_dispatcher_run(app->view_dispatcher);
    FURI_LOG_I(TAG, "thread stopped");
    return 0;
}

FlipperECUGui* flipper_ecu_gui_alloc(void* main_app) {
    FlipperECUGui* app = malloc(sizeof(FlipperECUGui));
    FlipperECUApp* ecu_app = main_app;
    app->thread = furi_thread_alloc_ex(TAG, 1024, flipper_ecu_gui_thread, app);

    app->gui = furi_record_open(RECORD_GUI);
    app->scene_manager = scene_manager_alloc(&flipper_ecu_scene_handlers, app);
    app->view_dispatcher = view_dispatcher_alloc();
    app->submenu = submenu_alloc();
    app->widget = widget_alloc();

    app->view_dashboard = flipper_ecu_view_dashboard_alloc();

    view_dispatcher_enable_queue(app->view_dispatcher);

    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, flipper_ecu_gui_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, flipper_ecu_gui_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, flipper_ecu_gui_tick_event_callback, FLIPPER_ECU_GUI_TICK);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_add_view(
        app->view_dispatcher, FlipperECUGuiViewSubmenu, submenu_get_view(app->submenu));
    view_dispatcher_add_view(
        app->view_dispatcher, FlipperECUGuiViewWidget, widget_get_view(app->widget));
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipperECUGuiViewDashboard,
        flipper_ecu_view_dashboard_get_view(app->view_dashboard));
    scene_manager_next_scene(app->scene_manager, FlipperECUSceneStart);

    app->ecu_app = ecu_app;

    return app;
}

void flipper_ecu_gui_free(FlipperECUGui* app) {
    view_dispatcher_remove_view(app->view_dispatcher, FlipperECUGuiViewDashboard);
    view_dispatcher_remove_view(app->view_dispatcher, FlipperECUGuiViewSubmenu);
    view_dispatcher_remove_view(app->view_dispatcher, FlipperECUGuiViewWidget);

    flipper_ecu_view_dashboard_free(app->view_dashboard);
    widget_free(app->widget);
    submenu_free(app->submenu);

    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);
    furi_thread_free(app->thread);

    furi_record_close(RECORD_GUI);
    free(app);
}

void flipper_ecu_gui_start(FlipperECUGui* app) {
    furi_thread_start(app->thread);
}

void flipper_ecu_gui_await_stop(FlipperECUGui* app) {
    furi_thread_join(app->thread);
}
