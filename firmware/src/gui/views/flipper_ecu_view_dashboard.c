#include "flipper_ecu_view_dashboard.h"

#include <furi.h>

struct FlipperECUDashboardView {
    View* view;
};

typedef struct {
    const FlipperECUEngineStatus* engine_status;
    FlipperECUAdcWorker* adc_worker;
    uint8_t page;
} FlipperECUDashboardViewModel;

static void flipper_ecu_view_dashboard_draw_page0(
    Canvas* canvas,
    FlipperECUDashboardViewModel* view_dashboard_model) {
    FuriString* fstr = furi_string_alloc();

    furi_string_printf(fstr, "RPM: %u", view_dashboard_model->engine_status->rpm);
    canvas_draw_str(canvas, 0, 10, furi_string_get_cstr(fstr));

    furi_string_printf(fstr, "Ignition angle: %u", view_dashboard_model->engine_status->ign_angle);
    canvas_draw_str(canvas, 0, 20, furi_string_get_cstr(fstr));

    furi_string_printf(
        fstr, "Sync status: %s", view_dashboard_model->engine_status->synced ? "true" : "false");
    canvas_draw_str(canvas, 0, 30, furi_string_get_cstr(fstr));

    furi_string_printf(
        fstr, "Inj dead time: %.3f ms", view_dashboard_model->engine_status->inj_dead_time);
    canvas_draw_str(canvas, 0, 40, furi_string_get_cstr(fstr));

    furi_string_printf(fstr, "Inj time: %.3f ms", view_dashboard_model->engine_status->inj_time);
    canvas_draw_str(canvas, 0, 50, furi_string_get_cstr(fstr));

    furi_string_free(fstr);
}

static void flipper_ecu_view_dashboard_draw_page1(
    Canvas* canvas,
    FlipperECUDashboardViewModel* view_dashboard_model) {
    FuriString* fstr = furi_string_alloc();

    furi_string_printf(
        fstr,
        "TPS ADC: %.0f mV",
        flipper_ecu_adc_worker_get_value_tps(view_dashboard_model->adc_worker));
    canvas_draw_str(canvas, 0, 10, furi_string_get_cstr(fstr));

    furi_string_printf(
        fstr,
        "TPS value: %.0f %%",
        flipper_ecu_adc_worker_get_value_tps_full(view_dashboard_model->adc_worker));
    canvas_draw_str(canvas, 0, 20, furi_string_get_cstr(fstr));

    furi_string_printf(
        fstr,
        "MAP ADC: %.0f mV",
        flipper_ecu_adc_worker_get_value_map(view_dashboard_model->adc_worker));
    canvas_draw_str(canvas, 0, 30, furi_string_get_cstr(fstr));

    furi_string_printf(
        fstr,
        "MAP value: %.3f kP",
        flipper_ecu_adc_worker_get_value_map_full(view_dashboard_model->adc_worker));
    canvas_draw_str(canvas, 0, 40, furi_string_get_cstr(fstr));

    furi_string_printf(
        fstr,
        "Air temp ADC: %.0f mV",
        flipper_ecu_adc_worker_get_value_air_temp(view_dashboard_model->adc_worker));
    canvas_draw_str(canvas, 0, 50, furi_string_get_cstr(fstr));

    furi_string_printf(
        fstr,
        "Air temp: %.0f C",
        flipper_ecu_adc_worker_get_value_air_temp_full(view_dashboard_model->adc_worker));
    canvas_draw_str(canvas, 0, 60, furi_string_get_cstr(fstr));

    furi_string_free(fstr);
}

static void flipper_ecu_view_dashboard_draw_page2(
    Canvas* canvas,
    FlipperECUDashboardViewModel* view_dashboard_model) {
    FuriString* fstr = furi_string_alloc();

    furi_string_printf(
        fstr,
        "Vbat: %.0f mV",
        flipper_ecu_adc_worker_get_value_vbat(view_dashboard_model->adc_worker));
    canvas_draw_str(canvas, 0, 10, furi_string_get_cstr(fstr));

    furi_string_printf(
        fstr,
        "Sync processing ticks: %lu",
        view_dashboard_model->engine_status->sync_processing_time);
    canvas_draw_str(canvas, 0, 20, furi_string_get_cstr(fstr));

    furi_string_printf(
        fstr,
        "First gpio tick: %lu",
        view_dashboard_model->engine_status->first_queue_event_ticks);
    canvas_draw_str(canvas, 0, 30, furi_string_get_cstr(fstr));

    double ckps_sync_isr_processing_speed_ratio = 0;
    if(view_dashboard_model->engine_status->first_queue_event_ticks > 0) {
        ckps_sync_isr_processing_speed_ratio =
            ((double)view_dashboard_model->engine_status->sync_processing_time /
             (double)view_dashboard_model->engine_status->first_queue_event_ticks) *
            100;
    }
    furi_string_printf(
        fstr, "Sync ISR speed ratio: %.2f %%", ckps_sync_isr_processing_speed_ratio);
    canvas_draw_str(canvas, 0, 40, furi_string_get_cstr(fstr));

    furi_string_free(fstr);
}

static void flipper_ecu_view_dashboard_draw_callback(Canvas* canvas, void* _model) {
    FlipperECUDashboardViewModel* view_dashboard_model = _model;
    canvas_clear(canvas);
    if(view_dashboard_model->page == 0) {
        flipper_ecu_view_dashboard_draw_page0(canvas, view_dashboard_model);
    } else if(view_dashboard_model->page == 1) {
        flipper_ecu_view_dashboard_draw_page1(canvas, view_dashboard_model);
    } else if(view_dashboard_model->page == 2) {
        flipper_ecu_view_dashboard_draw_page2(canvas, view_dashboard_model);
    }
}

static bool flipper_ecu_view_dashboard_input_callback(InputEvent* event, void* _model) {
    furi_assert(_model);
    FlipperECUDashboardView* view_dashboard = _model;
    bool consumed = false;
    with_view_model(
        view_dashboard->view,
        FlipperECUDashboardViewModel * model,
        {
            if((event->type == InputTypeShort) || (event->type == InputTypeRepeat)) {
                if(event->key == InputKeyRight) {
                    if(model->page < 2) {
                        model->page++;
                        consumed = true;
                    }
                }
                if(event->key == InputKeyLeft) {
                    if(model->page > 0) {
                        model->page--;
                        consumed = true;
                    }
                }
            }
        },
        true);
    return consumed;
}

FlipperECUDashboardView* flipper_ecu_view_dashboard_alloc(
    FlipperECUApp* ecu_app,
    const FlipperECUEngineStatus* engine_status) {
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
        {
            model->engine_status = engine_status;
            model->adc_worker = flipper_ecu_app_get_adc_worker(ecu_app);
            model->page = 0;
        },
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
