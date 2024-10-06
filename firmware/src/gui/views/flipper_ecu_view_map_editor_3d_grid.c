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
    uint32_t selected_horizontal_i;
    uint32_t selected_vertical_i;
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
        },
        true);
    flipper_ecu_map_print_to_logs_3d(map);
}

void flipper_ecu_view_map_editor_3d_grid_reset(
    FlipperECUMapEditor3DGridView* view_map_editor_3d_grid) {
    with_view_model(
        view_map_editor_3d_grid->view,
        FlipperECUMapEditor3DGridViewModel * model,
        {
            model->map = NULL;
            model->edit_mode = false;
        },
        false);
}
static void flipper_ecu_view_map_editor_3d_grid_draw_callback(Canvas* canvas, void* _model) {
    FlipperECUMapEditor3DGridViewModel* model = _model;

    uint32_t map_size_x = flipper_ecu_map_get_map_x_size(model->map);
    uint32_t map_size_z = flipper_ecu_map_get_map_z_size_3d(model->map);
    uint32_t cell_size_horizontal = DISPLAY_WIDTH / map_size_x;
    uint32_t cell_size_vertical = DISPLAY_HEIGHT / map_size_z;
    int16_t last_used_key_x1 = flipper_ecu_map_get_last_used_key_x1(model->map);
    int16_t last_used_key_x2 = flipper_ecu_map_get_last_used_key_x2(model->map);
    int16_t last_used_key_z1 = flipper_ecu_map_get_last_used_key_z1_3d(model->map);
    int16_t last_used_key_z2 = flipper_ecu_map_get_last_used_key_z2_3d(model->map);

    canvas_clear(canvas);

    if(!model->edit_mode) {
        for(uint32_t vertical_i = 0; vertical_i < map_size_z; vertical_i++) { // rows
            int16_t key_z = flipper_ecu_map_get_key_z_by_index_3d(model->map, vertical_i);
            for(uint32_t horizontal_i = 0; horizontal_i < map_size_x; horizontal_i++) { // columns
                int16_t key_x = flipper_ecu_map_get_key_x_by_index(model->map, horizontal_i);
                bool in_use = false;
                bool is_cell_selected = false;
                if(((key_x == last_used_key_x1) || (key_x == last_used_key_x2)) &&
                   ((key_z == last_used_key_z1) || (key_z == last_used_key_z2))) {
                    in_use = true;
                }
                if((horizontal_i == model->selected_horizontal_i) &&
                   vertical_i == model->selected_vertical_i) {
                    is_cell_selected = true;
                }
                // draw cell
                uint8_t temp_ill_remove_it_x = horizontal_i != (map_size_x - 1);
                uint8_t temp_ill_remove_it_y = vertical_i != (map_size_z - 1);
                if(in_use || is_cell_selected) {
                    canvas_draw_box(
                        canvas,
                        cell_size_horizontal * horizontal_i,
                        cell_size_vertical * vertical_i,
                        cell_size_horizontal + temp_ill_remove_it_x,
                        cell_size_vertical + temp_ill_remove_it_y);
                } else {
                    canvas_draw_frame(
                        canvas,
                        cell_size_horizontal * horizontal_i,
                        cell_size_vertical * vertical_i,
                        cell_size_horizontal + temp_ill_remove_it_x,
                        cell_size_vertical + temp_ill_remove_it_y);
                }
            }
        }
    } else { // edit mode
        FuriString* fstr = furi_string_alloc();
        int16_t value = flipper_ecu_map_get_value_by_index_3d(
            model->map, model->selected_horizontal_i, model->selected_vertical_i);
        int16_t key_x =
            flipper_ecu_map_get_key_x_by_index(model->map, model->selected_horizontal_i);
        int16_t key_z =
            flipper_ecu_map_get_key_z_by_index_3d(model->map, model->selected_vertical_i);
        const char* map_x_name = flipper_ecu_map_get_x_name(model->map);
        const char* map_z_name = flipper_ecu_map_get_z_name_3d(model->map);
        const char* map_values_name = flipper_ecu_map_get_values_name(model->map);
        furi_string_printf(fstr, "Current %s: %d", map_x_name, key_x);
        canvas_draw_str_aligned(canvas, 0, 0, AlignLeft, AlignTop, furi_string_get_cstr(fstr));
        furi_string_printf(fstr, "Current %s: %d", map_z_name, key_z);
        canvas_draw_str_aligned(canvas, 0, 10, AlignLeft, AlignTop, furi_string_get_cstr(fstr));
        furi_string_printf(fstr, "Current %s: %d", map_values_name, value);
        canvas_draw_str_aligned(canvas, 0, 20, AlignLeft, AlignTop, furi_string_get_cstr(fstr));
        furi_string_free(fstr);
    }
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
                    } else if(event->key == InputKeyRight) {
                        uint32_t map_size_x = flipper_ecu_map_get_map_x_size(model->map);
                        if(model->selected_horizontal_i < map_size_x - 1) {
                            model->selected_horizontal_i++;
                            consumed = true;
                        }
                    } else if(event->key == InputKeyLeft) {
                        if(model->selected_horizontal_i > 0) {
                            model->selected_horizontal_i--;
                            consumed = true;
                        }
                    } else if(event->key == InputKeyUp) {
                        if(model->selected_vertical_i > 0) {
                            model->selected_vertical_i--;
                            consumed = true;
                        }
                    } else if(event->key == InputKeyDown) {
                        uint32_t map_size_z = flipper_ecu_map_get_map_z_size_3d(model->map);
                        if(model->selected_vertical_i < map_size_z - 1) {
                            model->selected_vertical_i++;
                            consumed = true;
                        }
                    }
                }
            } else { // edit mode
                if((event->type == InputTypeShort) || (event->type == InputTypeRepeat)) {
                    int16_t value = flipper_ecu_map_get_value_by_index_3d(
                        model->map, model->selected_horizontal_i, model->selected_vertical_i);
                    int16_t value_max = flipper_ecu_map_get_value_max(model->map);
                    int16_t value_min = flipper_ecu_map_get_value_min(model->map);
                    if(event->key == InputKeyBack) {
                        model->edit_mode = false;
                        consumed = true;
                    } else if(event->key == InputKeyUp) {
                        if(event->type == InputTypeShort) {
                            if(value < value_max) {
                                flipper_ecu_map_set_value_by_index_3d(
                                    model->map,
                                    model->selected_horizontal_i,
                                    model->selected_vertical_i,
                                    value + 1);
                            }
                        } else if(event->type == InputTypeRepeat) {
                            if(value < value_max - 10) {
                                flipper_ecu_map_set_value_by_index_3d(
                                    model->map,
                                    model->selected_horizontal_i,
                                    model->selected_vertical_i,
                                    value + 10);
                            }
                        }
                        consumed = true;
                    } else if(event->key == InputKeyDown) {
                        if(event->type == InputTypeShort) {
                            if(value > value_min) {
                                flipper_ecu_map_set_value_by_index_3d(
                                    model->map,
                                    model->selected_horizontal_i,
                                    model->selected_vertical_i,
                                    value - 1);
                            }
                        } else if(event->type == InputTypeRepeat) {
                            if(value > value_min + 10) {
                                flipper_ecu_map_set_value_by_index_3d(
                                    model->map,
                                    model->selected_horizontal_i,
                                    model->selected_vertical_i,
                                    value - 10);
                            }
                        }
                        consumed = true;
                    }
                }
            }
        },
        true);
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
