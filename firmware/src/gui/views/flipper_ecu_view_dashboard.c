#include "flipper_ecu_view_dashboard.h"

#include <furi.h>

struct FlipperECUDashboardView {
    View* view;
};

typedef struct {
    uint32_t rpm;
} FlipperECUDashboardViewModel;

static void flipper_ecu_view_dashboard_draw_callback(Canvas* canvas, void* _model) {
    FlipperECUDashboardViewModel* view_dashboard_model = _model;
    char temp_str[18];
    canvas_clear(canvas);
    snprintf(temp_str, 18, "RPM: %lu", view_dashboard_model->rpm);
    canvas_draw_str(canvas, 10, 10, temp_str);
}

static bool flipper_ecu_view_dashboard_input_callback(InputEvent* event, void* _model) {
    furi_assert(_model);
    FlipperECUDashboardViewModel* view_dashboard_model = _model;
    UNUSED(view_dashboard_model);
    UNUSED(event);
    bool consumed = false;
    return consumed;
}

FlipperECUDashboardView* flipper_ecu_view_dashboard_alloc(void) {
    FlipperECUDashboardView* view_dashboard = malloc(sizeof(FlipperECUDashboardView));
    view_dashboard->view = view_alloc();
    view_allocate_model(
        view_dashboard->view, ViewModelTypeLocking, sizeof(FlipperECUDashboardViewModel));
    view_set_context(view_dashboard->view, view_dashboard);
    view_set_draw_callback(view_dashboard->view, flipper_ecu_view_dashboard_draw_callback);
    view_set_input_callback(view_dashboard->view, flipper_ecu_view_dashboard_input_callback);
    return view_dashboard;
}

void flipper_ecu_view_dashboard_free(FlipperECUDashboardView* view_dashboard) {
    view_free(view_dashboard->view);
    free(view_dashboard);
}

void flipper_ecu_view_dashboard_set_rpm(FlipperECUDashboardView* view_dashboard, uint32_t rpm) {
    with_view_model(
        view_dashboard->view, FlipperECUDashboardViewModel * model, { model->rpm = rpm; }, true);
}

void flipper_ecu_view_dashboard_view_update(FlipperECUDashboardView* view_dashboard) {
    with_view_model(
        view_dashboard->view, FlipperECUDashboardViewModel * model, { UNUSED(model); }, true);
}

View* flipper_ecu_view_dashboard_get_view(FlipperECUDashboardView* view_dashboard) {
    return view_dashboard->view;
}
