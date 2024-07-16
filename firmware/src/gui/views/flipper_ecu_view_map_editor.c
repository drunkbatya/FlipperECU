#include "flipper_ecu_view_map_editor.h"

#include <furi.h>

#define MAP_NAME_SIZE 20
#define X_Y_NAME_SIZE 10

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define FONT_HEIGHT 10

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
            model->x_end = 10000;
            model->y_end = 20;
            strncpy(model->map_name, "Ignition main map", MAP_NAME_SIZE - 1);
            strncpy(model->x_name, "RPM", X_Y_NAME_SIZE - 1);
            strncpy(model->y_name, "Deg", X_Y_NAME_SIZE - 1);
        },
        true);
}

static void process_number_to_fstr(FuriString* fstr, int32_t number) {
    furi_string_printf(fstr, "%ld", number);
}

static void flipper_ecu_view_map_editor_draw_callback(Canvas* canvas, void* _model) {
    FlipperECUMapEditorViewModel* map_editor_model = _model;
    const uint8_t main_field_start_x = FONT_HEIGHT;
    const uint8_t main_field_start_y = FONT_HEIGHT;
    const uint8_t main_field_width = DISPLAY_WIDTH - FONT_HEIGHT; // Y values
    const uint8_t main_field_height = DISPLAY_HEIGHT - (FONT_HEIGHT * 2); // title and X values

    FuriString* fstr = furi_string_alloc();

    canvas_draw_frame(
        canvas, main_field_start_x, main_field_start_y, main_field_width, main_field_height);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(
        canvas, (DISPLAY_WIDTH / 2), 0, AlignCenter, AlignTop, map_editor_model->map_name);

    uint16_t step = main_field_width / (map_editor_model->size_x - 1);
    uint16_t step_rem = main_field_width % (map_editor_model->size_x - 1);
    uint16_t num_step =
        (map_editor_model->x_end - map_editor_model->x_start) / (map_editor_model->size_x - 1);
    for(uint8_t x_model = 0; x_model < map_editor_model->size_x; x_model++) {
        if(x_model == (map_editor_model->size_x - 1)) { // last iteration
            step += step_rem;
        }
        uint16_t x = (x_model * step) + main_field_start_x;
        canvas_draw_line(
            canvas, x, main_field_start_y, x, main_field_start_y + main_field_height - 1);
        if(x_model != (map_editor_model->size_x - 1)) { // ommiting last iteration
            process_number_to_fstr(fstr, (map_editor_model->x_start + (num_step * x_model)));
            canvas_draw_str_aligned(
                canvas,
                x,
                main_field_start_y + main_field_height,
                AlignCenter,
                AlignTop,
                furi_string_get_cstr(fstr));
        }
    }
    canvas_draw_str_aligned(
        canvas,
        DISPLAY_WIDTH,
        main_field_start_y + main_field_height,
        AlignRight,
        AlignTop,
        map_editor_model->x_name);
    furi_string_free(fstr);
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
