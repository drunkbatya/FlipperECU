#include "flipper_ecu_view_map_editor.h"

#include <furi.h>

#define MAP_NAME_SIZE 20
#define X_Y_NAME_SIZE 10

static const uint8_t main_field_start_x = 6;
static const uint8_t main_field_start_y = 8;
//static const uint8_t main_field_start_x = 0;
//static const uint8_t main_field_start_y = 0;
static const uint8_t main_field_width = 128 - 6;
static const uint8_t main_field_height = 40;
//static const uint8_t main_field_width = 128;
//static const uint8_t main_field_height = 64;

struct FlipperECUMapEditorView {
    View* view;
};

typedef struct {
    uint8_t size_x;
    uint8_t size_y;
    int32_t x_start;
    int32_t x_end;
    int32_t y_start;
    int32_t y_end;
    char map_name[MAP_NAME_SIZE];
    char x_name[X_Y_NAME_SIZE];
    char y_name[X_Y_NAME_SIZE];
} FlipperECUMapEditorViewModel;

void flipper_ecu_view_map_editor_load_map(FlipperECUMapEditorView* view_map_editor) {
    with_view_model(
        view_map_editor->view,
        FlipperECUMapEditorViewModel * model,
        {
            model->size_x = 5;
            model->size_y = 4;
            model->x_start = 0;
            model->y_start = 0;
            model->x_end = 20;
            model->y_end = 20;
            strncpy(model->map_name, "Ignition main map", MAP_NAME_SIZE - 1);
            strncpy(model->x_name, "RPM", X_Y_NAME_SIZE - 1);
            strncpy(model->y_name, "Deg", X_Y_NAME_SIZE - 1);
        },
        true);
}

static void flipper_ecu_view_map_editor_draw_callback(Canvas* canvas, void* _model) {
    FlipperECUMapEditorViewModel* map_editor_model = _model;
    canvas_draw_frame(
        canvas, main_field_start_x, main_field_start_y, main_field_width, main_field_height);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, map_editor_model->map_name);

    uint16_t step = main_field_width / (map_editor_model->size_x - 1);
    uint16_t step_rem = main_field_width % (map_editor_model->size_x - 1);
    for(uint8_t x_model = 0; x_model < map_editor_model->size_x; x_model++) {
        if(x_model == (map_editor_model->size_x - 1)) { // last iteration
            step += step_rem;
        }
        uint16_t x = (x_model * step) + main_field_start_x;
        canvas_draw_line(canvas, x, main_field_start_y, x, main_field_start_y + main_field_height);
    }
}

static bool flipper_ecu_view_map_editor_input_callback(InputEvent* event, void* _model) {
    furi_assert(_model);
    FlipperECUMapEditorViewModel* map_editor_model = _model;
    UNUSED(map_editor_model);
    UNUSED(event);
    bool consumed = false;
    return consumed;
}

FlipperECUMapEditorView* flipper_ecu_view_map_editor_alloc(void) {
    FlipperECUMapEditorView* map_editor = malloc(sizeof(FlipperECUMapEditorView));
    map_editor->view = view_alloc();
    view_allocate_model(
        map_editor->view, ViewModelTypeLocking, sizeof(FlipperECUMapEditorViewModel));
    view_set_context(map_editor->view, map_editor);
    view_set_draw_callback(map_editor->view, flipper_ecu_view_map_editor_draw_callback);
    view_set_input_callback(map_editor->view, flipper_ecu_view_map_editor_input_callback);
    return map_editor;
}

void flipper_ecu_view_map_editor_free(FlipperECUMapEditorView* map_editor) {
    view_free(map_editor->view);
    free(map_editor);
}

void flipper_ecu_view_map_editor_view_update(FlipperECUMapEditorView* map_editor) {
    with_view_model(
        map_editor->view, FlipperECUMapEditorViewModel * model, { UNUSED(model); }, true);
}

View* flipper_ecu_view_map_editor_get_view(FlipperECUMapEditorView* map_editor) {
    return map_editor->view;
}
