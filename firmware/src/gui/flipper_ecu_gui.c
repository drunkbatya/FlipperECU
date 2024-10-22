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

FlipperECUGui* flipper_ecu_gui_alloc(FlipperECUApp* ecu_app) {
    FlipperECUGui* app = malloc(sizeof(FlipperECUGui));
    app->thread = furi_thread_alloc_ex(TAG, 1024, flipper_ecu_gui_thread, app);

    app->gui = furi_record_open(RECORD_GUI);
    app->scene_manager = scene_manager_alloc(&flipper_ecu_scene_handlers, app);
    app->view_dispatcher = view_dispatcher_alloc();
    app->submenu = submenu_alloc();
    app->widget = widget_alloc();
    app->popup = popup_alloc();
    app->dialogs = furi_record_open(RECORD_DIALOGS);
    app->var_item_list = variable_item_list_alloc();
    app->text_input = text_input_alloc();
    app->file_path = furi_string_alloc_set(STORAGE_APP_DATA_PATH_PREFIX);

    app->view_dashboard = flipper_ecu_view_dashboard_alloc(ecu_app, &ecu_app->engine_status);
    app->view_map_editor = flipper_ecu_view_map_editor_alloc();
    app->view_map_editor_3d_grid = flipper_ecu_view_map_editor_3d_grid_alloc();
    app->view_sensor_config = flipper_ecu_view_sensor_config_alloc();
    app->view_idle_manual = flipper_ecu_view_idle_manual_alloc(ecu_app);

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
        app->view_dispatcher, FlipperECUGuiViewPopup, popup_get_view(app->popup));
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipperECUGuiViewDashboard,
        flipper_ecu_view_dashboard_get_view(app->view_dashboard));
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipperECUGuiViewMapEditor,
        flipper_ecu_view_map_editor_get_view(app->view_map_editor));
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipperECUGuiViewMapEditor3DGrid,
        flipper_ecu_view_map_editor_3d_grid_get_view(app->view_map_editor_3d_grid));
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipperECUGuiViewSensorConfig,
        flipper_ecu_view_sensor_config_get_view(app->view_sensor_config));
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipperECUGuiViewIdleManual,
        flipper_ecu_view_idle_manual_get_view(app->view_idle_manual));
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipperECUGuiViewVarItemList,
        variable_item_list_get_view(app->var_item_list));
    view_dispatcher_add_view(
        app->view_dispatcher, FlipperECUGuiViewTextInput, text_input_get_view(app->text_input));
    scene_manager_next_scene(app->scene_manager, FlipperECUSceneStart);

    furi_string_set(app->file_path, STORAGE_APP_DATA_PATH_PREFIX);

    app->ecu_app = ecu_app;

    return app;
}

void flipper_ecu_gui_free(FlipperECUGui* app) {
    view_dispatcher_remove_view(app->view_dispatcher, FlipperECUGuiViewDashboard);
    view_dispatcher_remove_view(app->view_dispatcher, FlipperECUGuiViewMapEditor3DGrid);
    view_dispatcher_remove_view(app->view_dispatcher, FlipperECUGuiViewMapEditor);
    view_dispatcher_remove_view(app->view_dispatcher, FlipperECUGuiViewSensorConfig);
    view_dispatcher_remove_view(app->view_dispatcher, FlipperECUGuiViewIdleManual);
    view_dispatcher_remove_view(app->view_dispatcher, FlipperECUGuiViewSubmenu);
    view_dispatcher_remove_view(app->view_dispatcher, FlipperECUGuiViewWidget);
    view_dispatcher_remove_view(app->view_dispatcher, FlipperECUGuiViewPopup);
    view_dispatcher_remove_view(app->view_dispatcher, FlipperECUGuiViewVarItemList);
    view_dispatcher_remove_view(app->view_dispatcher, FlipperECUGuiViewTextInput);

    flipper_ecu_view_dashboard_free(app->view_dashboard);
    flipper_ecu_view_map_editor_3d_grid_free(app->view_map_editor_3d_grid);
    flipper_ecu_view_map_editor_free(app->view_map_editor);
    flipper_ecu_view_sensor_config_free(app->view_sensor_config);
    flipper_ecu_view_idle_manual_free(app->view_idle_manual);

    widget_free(app->widget);
    popup_free(app->popup);
    submenu_free(app->submenu);
    variable_item_list_free(app->var_item_list);
    text_input_free(app->text_input);
    furi_string_free(app->file_path);

    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);
    furi_thread_free(app->thread);

    furi_record_close(RECORD_DIALOGS);
    furi_record_close(RECORD_GUI);
    free(app);
}

void flipper_ecu_gui_start(FlipperECUGui* app) {
    furi_thread_start(app->thread);
}

void flipper_ecu_gui_await_stop(FlipperECUGui* app) {
    furi_thread_join(app->thread);
}
