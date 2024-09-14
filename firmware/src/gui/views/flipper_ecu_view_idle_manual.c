#include "flipper_ecu_view_idle_manual.h"

#include <gui/elements.h>

#include <furi.h>

struct FlipperECUIdleManualView {
    View* view;
};

typedef struct {
    FuriString* fstr;
    FlipperECUIdleValveWorker* idle_valve_worker;
    FlipperECUEngineSettings* engine_settings;
} FlipperECUIdleManualViewModel;

static void flipper_ecu_view_idle_manual_draw_callback(Canvas* canvas, void* _model) {
    FlipperECUIdleManualViewModel* view_idle_manual_model = _model;

    canvas_clear(canvas);

    uint16_t idle_pos = flipper_ecu_idle_valve_worker_get_current_position(
        view_idle_manual_model->idle_valve_worker);
    uint16_t idle_total_steps = view_idle_manual_model->engine_settings->idle_valve_total_steps;

    canvas_draw_str_aligned(canvas, 0, 0, AlignLeft, AlignTop, "Current position:");
    furi_string_printf(view_idle_manual_model->fstr, "%d steps", idle_pos);
    canvas_draw_str_aligned(
        canvas, 128, 0, AlignRight, AlignTop, furi_string_get_cstr(view_idle_manual_model->fstr));

    canvas_draw_str_aligned(canvas, 0, 10, AlignLeft, AlignTop, "Maximum:");
    furi_string_printf(view_idle_manual_model->fstr, "%d steps", idle_total_steps);
    canvas_draw_str_aligned(
        canvas, 128, 10, AlignRight, AlignTop, furi_string_get_cstr(view_idle_manual_model->fstr));

    if(idle_pos > 0) {
        elements_button_left(canvas, "step");
    }
    if(idle_pos < idle_total_steps) {
        elements_button_right(canvas, "step");
    }
    elements_button_center(canvas, "full");
}

static bool flipper_ecu_view_idle_manual_input_callback(InputEvent* event, void* _model) {
    furi_assert(_model);
    FlipperECUIdleManualView* view_idle_manual = _model;
    bool consumed = false;
    with_view_model(
        view_idle_manual->view,
        FlipperECUIdleManualViewModel * model,
        {
            if((event->type == InputTypeShort) || (event->type == InputTypeRepeat)) {
                uint16_t idle_pos =
                    flipper_ecu_idle_valve_worker_get_current_position(model->idle_valve_worker);
                uint16_t idle_total_steps = model->engine_settings->idle_valve_total_steps;
                if(event->key == InputKeyRight) {
                    if(idle_pos < idle_total_steps) {
                        flipper_ecu_idle_valve_worker_step(
                            model->idle_valve_worker, IdleValveDirectionForward);
                        consumed = true;
                    }
                }
                if(event->key == InputKeyLeft) {
                    if(idle_pos > 0) {
                        flipper_ecu_idle_valve_worker_step(
                            model->idle_valve_worker, IdleValveDirectionBackward);
                        consumed = true;
                    }
                }
                if(event->key == InputKeyOk) {
                    flipper_ecu_idle_valve_worker_calibrate(model->idle_valve_worker);
                    consumed = true;
                }
            }
        },
        true);
    return consumed;
}

FlipperECUIdleManualView* flipper_ecu_view_idle_manual_alloc(FlipperECUApp* ecu_app) {
    FlipperECUIdleManualView* view_idle_manual = malloc(sizeof(FlipperECUIdleManualView));
    view_idle_manual->view = view_alloc();
    view_allocate_model(
        view_idle_manual->view, ViewModelTypeLocking, sizeof(FlipperECUIdleManualViewModel));
    view_set_context(view_idle_manual->view, view_idle_manual);
    view_set_draw_callback(view_idle_manual->view, flipper_ecu_view_idle_manual_draw_callback);
    view_set_input_callback(view_idle_manual->view, flipper_ecu_view_idle_manual_input_callback);

    with_view_model(
        view_idle_manual->view,
        FlipperECUIdleManualViewModel * model,
        {
            model->fstr = furi_string_alloc();
            model->idle_valve_worker = flipper_ecu_app_get_idle_valve_worker(ecu_app);
            model->engine_settings = flipper_ecu_app_get_engine_settings(ecu_app);
        },
        false);

    return view_idle_manual;
}

void flipper_ecu_view_idle_manual_free(FlipperECUIdleManualView* view_idle_manual) {
    with_view_model(
        view_idle_manual->view,
        FlipperECUIdleManualViewModel * model,
        { furi_string_free(model->fstr); },
        false);
    view_free(view_idle_manual->view);
    free(view_idle_manual);
}

void flipper_ecu_view_idle_manual_view_update(FlipperECUIdleManualView* view_idle_manual) {
    with_view_model(
        view_idle_manual->view, FlipperECUIdleManualViewModel * model, { UNUSED(model); }, true);
}

View* flipper_ecu_view_idle_manual_get_view(FlipperECUIdleManualView* view_idle_manual) {
    return view_idle_manual->view;
}
