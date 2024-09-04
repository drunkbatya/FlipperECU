#include "flipper_ecu_view_sensor_config.h"

#include <furi.h>

struct FlipperECUSensorConfigView {
    View* view;
};

typedef struct {
    FuriString* fstr;
    SensorType sensor_type;
    const float* adc_raw_ptr;
    const float* adc_converted_value;
} FlipperECUSensorConfigViewModel;

static void flipper_ecu_view_sensor_config_draw_callback_tps(
    Canvas* canvas,
    FlipperECUSensorConfigViewModel* view_sensor_config_model) {
    canvas_clear(canvas);

    canvas_draw_str_aligned(canvas, 0, 0, AlignLeft, AlignTop, "ADC voltage:");
    furi_string_printf(
        view_sensor_config_model->fstr, "%f mV", (double)*view_sensor_config_model->adc_raw_ptr);
    canvas_draw_str_aligned(
        canvas, 128, 0, AlignRight, AlignTop, furi_string_get_cstr(view_sensor_config_model->fstr));

    canvas_draw_str_aligned(canvas, 0, 10, AlignLeft, AlignTop, "Will be");
    furi_string_printf(view_sensor_config_model->fstr, "%d %%", 100);
    canvas_draw_str_aligned(
        canvas,
        128,
        10,
        AlignRight,
        AlignTop,
        furi_string_get_cstr(view_sensor_config_model->fstr));
}

static void flipper_ecu_view_sensor_config_draw_callback(Canvas* canvas, void* _model) {
    FlipperECUSensorConfigViewModel* view_sensor_config_model = _model;
    if(view_sensor_config_model->sensor_type == SENSOR_TYPE_TPS) {
        flipper_ecu_view_sensor_config_draw_callback_tps(canvas, view_sensor_config_model);
    } else {
    }
}

static bool flipper_ecu_view_sensor_config_input_callback(InputEvent* event, void* _model) {
    furi_assert(_model);
    FlipperECUSensorConfigViewModel* view_sensor_config_model = _model;
    UNUSED(view_sensor_config_model);
    UNUSED(event);
    bool consumed = false;
    return consumed;
}

FlipperECUSensorConfigView* flipper_ecu_view_sensor_config_alloc(void) {
    FlipperECUSensorConfigView* view_sensor_config = malloc(sizeof(FlipperECUSensorConfigView));
    view_sensor_config->view = view_alloc();
    view_allocate_model(
        view_sensor_config->view, ViewModelTypeLocking, sizeof(FlipperECUSensorConfigViewModel));
    view_set_context(view_sensor_config->view, view_sensor_config);
    view_set_draw_callback(view_sensor_config->view, flipper_ecu_view_sensor_config_draw_callback);
    view_set_input_callback(
        view_sensor_config->view, flipper_ecu_view_sensor_config_input_callback);

    with_view_model(
        view_sensor_config->view,
        FlipperECUSensorConfigViewModel * model,
        { model->fstr = furi_string_alloc(); },
        false);

    flipper_ecu_view_sensor_config_reset(view_sensor_config);

    return view_sensor_config;
}

void flipper_ecu_view_sensor_config_free(FlipperECUSensorConfigView* view_sensor_config) {
    with_view_model(
        view_sensor_config->view,
        FlipperECUSensorConfigViewModel * model,
        { furi_string_free(model->fstr); },
        false);
    view_free(view_sensor_config->view);
    free(view_sensor_config);
}

void flipper_ecu_view_sensor_config_view_update(FlipperECUSensorConfigView* view_sensor_config) {
    with_view_model(
        view_sensor_config->view,
        FlipperECUSensorConfigViewModel * model,
        { UNUSED(model); },
        true);
}

View* flipper_ecu_view_sensor_config_get_view(FlipperECUSensorConfigView* view_sensor_config) {
    return view_sensor_config->view;
}

void flipper_ecu_view_sensor_config_set_sensor_type(
    FlipperECUSensorConfigView* view_sensor_config,
    SensorType type) {
    with_view_model(
        view_sensor_config->view,
        FlipperECUSensorConfigViewModel * model,
        { model->sensor_type = type; },
        false);
}

void flipper_ecu_view_sensor_config_reset(FlipperECUSensorConfigView* view_sensor_config) {
    with_view_model(
        view_sensor_config->view,
        FlipperECUSensorConfigViewModel * model,
        { model->sensor_type = SENSOR_TYPE_NONE; },
        false);
}

void flipper_ecu_view_sensor_config_set_adc_raw_ptr(
    FlipperECUSensorConfigView* view_sensor_config,
    float* ptr) {
    with_view_model(
        view_sensor_config->view,
        FlipperECUSensorConfigViewModel * model,
        { model->adc_raw_ptr = ptr; },
        false);
}
