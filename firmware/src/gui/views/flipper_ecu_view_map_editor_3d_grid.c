#include "flipper_ecu_view_map_editor_3d_grid.h"

#include <furi.h>
#include <gui/elements.h>

#include "../../flipper_ecu_map.h"

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define FONT_HEIGHT 10

#define EDITOR_ITEM_H 64 / 3
#define EDITOR_ITEM_W 128

#define PLOT_PADDING_PX 4

struct FlipperECUMapEditor3DGridView {
    View* view;
};

typedef struct {
    FlipperECUMap* map;
    uint32_t prev_dot_x;
    uint32_t prev_dot_y;
    uint16_t selected_x_dot;
    uint16_t selected_z_dot;
    bool edit_mode;
} FlipperECUMapEditor3DGridViewModel;

void flipper_ecu_view_map_editor_3d_grid_load_map(
    FlipperECUMapEditor3DGridView* view_map_editor_3d_grid,
    FlipperECUMap* map) {
    with_view_model(
        view_map_editor_3d_grid->view,
        FlipperECUMapEditor3DGridViewModel * model,
        {
            model->map = map;
            model->edit_mode = false;
            model->selected_x_dot = 3;
            model->selected_z_dot = 0;
        },
        true);
}

void flipper_ecu_view_map_editor_3d_grid_reset(
    FlipperECUMapEditor3DGridView* view_map_editor_3d_grid) {
    with_view_model(
        view_map_editor_3d_grid->view,
        FlipperECUMapEditor3DGridViewModel * model,
        {
            model->map = NULL;
            model->edit_mode = false;
            model->selected_x_dot = 0;
            model->selected_z_dot = 0;
        },
        false);
}

static void flipper_ecu_view_map_editor_3d_grid_draw_callback(Canvas* canvas, void* _model) {
    FlipperECUMapEditor3DGridViewModel* map_editor_model = _model;
    UNUSED(map_editor_model);

    canvas_draw_str_aligned(canvas, 0, 10, AlignLeft, AlignTop, "Fuck");
}

static bool flipper_ecu_view_map_editor_3d_grid_input_callback(InputEvent* event, void* context) {
    FlipperECUMapEditor3DGridView* view_map_editor = context;
    bool consumed = false;
    with_view_model(
        view_map_editor->view,
        FlipperECUMapEditor3DGridViewModel * model,
        {
            if(!model->edit_mode) {
                if((event->type == InputTypeShort) || (event->type == InputTypeRepeat)) {
                    if(event->key == InputKeyOk) {
                        model->edit_mode = true;
                        consumed = true;
                    }
                }
            }
        },
        false);
    return consumed;
}

FlipperECUMapEditor3DGridView* flipper_ecu_view_map_editor_3d_grid_alloc(void) {
    FlipperECUMapEditor3DGridView* map_editor = malloc(sizeof(FlipperECUMapEditor3DGridView));
    map_editor->view = view_alloc();
    view_allocate_model(
        map_editor->view, ViewModelTypeLocking, sizeof(FlipperECUMapEditor3DGridViewModel));
    view_set_context(map_editor->view, map_editor);
    view_set_draw_callback(map_editor->view, flipper_ecu_view_map_editor_3d_grid_draw_callback);
    view_set_input_callback(map_editor->view, flipper_ecu_view_map_editor_3d_grid_input_callback);
    return map_editor;
}

void flipper_ecu_view_map_editor_3d_grid_free(FlipperECUMapEditor3DGridView* map_editor) {
    view_free(map_editor->view);
    free(map_editor);
}

void flipper_ecu_view_map_editor_3d_grid_view_update(FlipperECUMapEditor3DGridView* map_editor) {
    with_view_model(
        map_editor->view, FlipperECUMapEditor3DGridViewModel * model, { UNUSED(model); }, true);
}

View* flipper_ecu_view_map_editor_3d_grid_get_view(FlipperECUMapEditor3DGridView* map_editor) {
    return map_editor->view;
}
