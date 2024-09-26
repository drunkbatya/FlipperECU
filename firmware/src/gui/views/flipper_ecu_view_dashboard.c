#include "flipper_ecu_view_dashboard.h"

#include <furi.h>

struct FlipperECUDashboardView {
    View* view;
};

typedef struct {
    const FlipperECUEngineStatus* engine_status;
} FlipperECUDashboardViewModel;

static void flipper_ecu_view_dashboard_draw_callback(Canvas* canvas, void* _model) {
    FlipperECUDashboardViewModel* view_dashboard_model = _model;
    FuriString* fstr = furi_string_alloc();

    canvas_clear(canvas);
    furi_string_printf(fstr, "RPM: %u", view_dashboard_model->engine_status->rpm);
    canvas_draw_str(canvas, 0, 10, furi_string_get_cstr(fstr));
    furi_string_printf(fstr, "Ignition angle: %u", view_dashboard_model->engine_status->ign_angle);
    canvas_draw_str(canvas, 0, 20, furi_string_get_cstr(fstr));
    furi_string_printf(
        fstr, "Sync status: %s", view_dashboard_model->engine_status->synced ? "true" : "false");
    canvas_draw_str(canvas, 0, 30, furi_string_get_cstr(fstr));
    furi_string_printf(fstr, "Maf value: %f kg/h", view_dashboard_model->engine_status->maf_value);
    canvas_draw_str(canvas, 0, 40, furi_string_get_cstr(fstr));
    furi_string_printf(fstr, "Inj time: %f ms", view_dashboard_model->engine_status->inj_time);
    canvas_draw_str(canvas, 0, 50, furi_string_get_cstr(fstr));
    furi_string_printf(fstr, "Maf ADC: %f mv", view_dashboard_model->engine_status->maf_adc);
    canvas_draw_str(canvas, 0, 60, furi_string_get_cstr(fstr));

    furi_string_free(fstr);
}

static bool flipper_ecu_view_dashboard_input_callback(InputEvent* event, void* _model) {
    furi_assert(_model);
    FlipperECUDashboardViewModel* view_dashboard_model = _model;
    UNUSED(view_dashboard_model);
    UNUSED(event);
    bool consumed = false;
    return consumed;
}

FlipperECUDashboardView*
    flipper_ecu_view_dashboard_alloc(const FlipperECUEngineStatus* engine_status) {
    FlipperECUDashboardView* view_dashboard = malloc(sizeof(FlipperECUDashboardView));
    view_dashboard->view = view_alloc();
    view_allocate_model(
        view_dashboard->view, ViewModelTypeLocking, sizeof(FlipperECUDashboardViewModel));
    view_set_context(view_dashboard->view, view_dashboard);
    view_set_draw_callback(view_dashboard->view, flipper_ecu_view_dashboard_draw_callback);
    view_set_input_callback(view_dashboard->view, flipper_ecu_view_dashboard_input_callback);
    with_view_model(
        view_dashboard->view,
        FlipperECUDashboardViewModel * model,
        { model->engine_status = engine_status; },
        true);
    return view_dashboard;
}

void flipper_ecu_view_dashboard_free(FlipperECUDashboardView* view_dashboard) {
    view_free(view_dashboard->view);
    free(view_dashboard);
}

void flipper_ecu_view_dashboard_view_update(FlipperECUDashboardView* view_dashboard) {
    with_view_model(
        view_dashboard->view, FlipperECUDashboardViewModel * model, { UNUSED(model); }, true);
}

View* flipper_ecu_view_dashboard_get_view(FlipperECUDashboardView* view_dashboard) {
    return view_dashboard->view;
}
