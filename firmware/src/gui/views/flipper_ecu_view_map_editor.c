#include "flipper_ecu_view_map_editor.h"

#include <furi.h>
#include <gui/elements.h>

#include "../../flipper_ecu_map.h"

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define FONT_HEIGHT 10

#define EDITOR_ITEM_H 64 / 3
#define EDITOR_ITEM_W 128

struct FlipperECUMapEditorView {
    View* view;
};

typedef struct {
    FlipperECUMap* map;
    uint8_t prev_dot_x;
    uint8_t prev_dot_y;
    uint8_t selected_x_dot;
    bool edit_mode;
    int16_t using_key;
    bool map_in_use;
} FlipperECUMapEditorViewModel;

void flipper_ecu_view_map_editor_load_map(
    FlipperECUMapEditorView* view_map_editor,
    FlipperECUMap* map) {
    with_view_model(
        view_map_editor->view,
        FlipperECUMapEditorViewModel * model,
        {
            model->map = map;
            model->edit_mode = false;
            model->selected_x_dot = 3;
            model->using_key = 1950;
            model->map_in_use = false;
        },
        true);
}

static void
    flipper_ecu_view_map_editor_draw_scaled_dot(Canvas* canvas, uint8_t x, uint8_t y, bool framed) {
    canvas_draw_dot(canvas, x, y);
    canvas_draw_dot(canvas, x - 1, y);
    canvas_draw_dot(canvas, x + 1, y);
    canvas_draw_dot(canvas, x, y - 1);
    canvas_draw_dot(canvas, x - 1, y - 1);
    canvas_draw_dot(canvas, x + 1, y - 1);
    canvas_draw_dot(canvas, x, y + 1);
    canvas_draw_dot(canvas, x - 1, y + 1);
    canvas_draw_dot(canvas, x + 1, y + 1);
    if(framed) {
        canvas_draw_frame(canvas, x - 3, y - 3, 7, 7);
    }
}

static void flipper_ecu_view_map_editor_draw_dotted_line(
    Canvas* canvas,
    uint8_t x1,
    uint8_t y1,
    uint8_t x2,
    uint8_t y2) {
    if(x1 == x2) { // vertical
        for(uint8_t i = y1; i < y2; i += 2) {
            canvas_draw_dot(canvas, x1, i);
        }
    } else if(y1 == y2) { // horizontal
        for(uint8_t i = x1; i < x2; i += 2) {
            canvas_draw_dot(canvas, i, y1);
        }
    } else {
        return; // not supported
    }
}

static void flipper_ecu_view_map_editor_draw_callback(Canvas* canvas, void* _model) {
    FlipperECUMapEditorViewModel* map_editor_model = _model;
    const uint8_t main_field_start_x = 0;
    const uint8_t main_field_start_y = FONT_HEIGHT;
    const uint8_t main_field_width = DISPLAY_WIDTH; // Y values
    const uint8_t main_field_height = DISPLAY_HEIGHT - (FONT_HEIGHT * 2) + 2; // title and X values
    FuriString* fstr = furi_string_alloc();

    // Table border
    canvas_draw_frame(
        canvas, main_field_start_x, main_field_start_y, main_field_width, main_field_height);

    // value name
    canvas_draw_str_aligned(
        canvas, 0, 0, AlignLeft, AlignTop, flipper_ecu_map_get_values_name(map_editor_model->map));

    // Map title
    canvas_draw_str_aligned(
        canvas,
        DISPLAY_WIDTH,
        0,
        AlignRight,
        AlignTop,
        flipper_ecu_map_get_map_name(map_editor_model->map));

    uint16_t step = main_field_width / (flipper_ecu_map_get_map_x_size(map_editor_model->map) - 1);
    uint16_t value_range = flipper_ecu_map_get_value_max(map_editor_model->map) -
                           flipper_ecu_map_get_value_min(map_editor_model->map);
    uint8_t scale = 1;
    uint8_t y_step = main_field_height / value_range;
    if(y_step == 0) {
        y_step = 1;
        if(main_field_height % value_range) {
            scale = value_range / main_field_height;
            if(value_range % main_field_height) {
                scale += 1;
            }
        }
    }
    for(uint8_t x_model = 0; x_model < flipper_ecu_map_get_map_x_size(map_editor_model->map);
        x_model++) {
        bool framed_dot = false;
        int16_t current_value_to_calc =
            flipper_ecu_map_get_value_by_index(map_editor_model->map, x_model);
        if(flipper_ecu_map_get_value_min(map_editor_model->map) < 0) {
            current_value_to_calc += -(flipper_ecu_map_get_value_min(map_editor_model->map));
        } else if(flipper_ecu_map_get_value_min(map_editor_model->map) > 0) {
            current_value_to_calc -= flipper_ecu_map_get_value_min(map_editor_model->map);
        }
        uint16_t x = (x_model * step) + main_field_start_x;
        uint8_t y =
            main_field_height - (current_value_to_calc * y_step / scale) + main_field_start_y - 1;

        // if engine is running and current key is last used
        //if(map_editor_model->map_in_use &&
        //   (map_editor_model->keys[x_model] == map_editor_model->using_key)) {
        //    framed_dot = true;
        //}

        // dots and line
        flipper_ecu_view_map_editor_draw_scaled_dot(canvas, x, y, framed_dot);
        if(x_model < flipper_ecu_map_get_map_x_size(map_editor_model->map) + 1) {
            if(x_model != 0) {
                canvas_draw_line(
                    canvas, map_editor_model->prev_dot_x, map_editor_model->prev_dot_y, x, y);
            }
            map_editor_model->prev_dot_x = x;
            map_editor_model->prev_dot_y = y;
        }

        // editing mode
        if(map_editor_model->edit_mode) {
            if(x_model == map_editor_model->selected_x_dot) {
                // lines
                flipper_ecu_view_map_editor_draw_dotted_line(
                    canvas, x, y, x, main_field_start_y + main_field_height - 1); // line to x
                flipper_ecu_view_map_editor_draw_dotted_line(
                    canvas, main_field_start_x, y, x, y); // line to y

                // Values
                furi_string_printf(
                    fstr,
                    "key: %d",
                    flipper_ecu_map_get_key_by_index(map_editor_model->map, x_model));
                canvas_draw_str_aligned(
                    canvas,
                    0,
                    DISPLAY_HEIGHT - 1,
                    AlignLeft,
                    AlignBottom,
                    furi_string_get_cstr(fstr));
                furi_string_printf(
                    fstr,
                    "value: %d",
                    flipper_ecu_map_get_value_by_index(map_editor_model->map, x_model));
                canvas_draw_str_aligned(
                    canvas,
                    45,
                    DISPLAY_HEIGHT - 1,
                    AlignLeft,
                    AlignBottom,
                    furi_string_get_cstr(fstr));
            }
        }
    }

    // x name
    canvas_draw_str_aligned(
        canvas,
        DISPLAY_WIDTH,
        DISPLAY_HEIGHT,
        AlignRight,
        AlignBottom,
        flipper_ecu_map_get_x_name(map_editor_model->map));
    furi_string_free(fstr);
}

static bool flipper_ecu_view_map_editor_input_callback(InputEvent* event, void* context) {
    FlipperECUMapEditorView* view_map_editor = context;
    bool consumed = false;
    with_view_model(
        view_map_editor->view,
        FlipperECUMapEditorViewModel * model,
        {
            if(!model->edit_mode) {
                if((event->type == InputTypeShort) || (event->type == InputTypeRepeat)) {
                    if(event->key == InputKeyOk) {
                        model->edit_mode = true;
                        consumed = true;
                    }
                }
            } else {
                if((event->type == InputTypeShort) || (event->type == InputTypeRepeat)) {
                    if((event->key == InputKeyOk) || (event->key == InputKeyBack)) {
                        if(event->type == InputTypeShort) {
                            model->edit_mode = false;
                            consumed = true;
                        }
                    } else if(event->key == InputKeyLeft) {
                        if(event->type == InputTypeShort) {
                            if(model->selected_x_dot != 0) {
                                model->selected_x_dot -= 1;
                            }
                            consumed = true;
                        } else if(event->type == InputTypeRepeat) {
                            if(model->selected_x_dot > 5) {
                                model->selected_x_dot -= 5;
                            } else if(model->selected_x_dot != 0) {
                                model->selected_x_dot -= 1;
                            }
                            consumed = true;
                        }
                    } else if(event->key == InputKeyRight) {
                        if(event->type == InputTypeShort) {
                            if(model->selected_x_dot <
                               flipper_ecu_map_get_map_x_size(model->map) - 1) {
                                model->selected_x_dot += 1;
                            }
                            consumed = true;
                        } else if(event->type == InputTypeRepeat) {
                            if(model->selected_x_dot <
                               flipper_ecu_map_get_map_x_size(model->map) - 5 - 1) {
                                model->selected_x_dot += 5;
                            } else if(
                                model->selected_x_dot <
                                flipper_ecu_map_get_map_x_size(model->map) - 1) {
                                model->selected_x_dot += 1;
                            }
                            consumed = true;
                        }
                    } else if(event->key == InputKeyUp) {
                        int16_t value =
                            flipper_ecu_map_get_value_by_index(model->map, model->selected_x_dot);
                        int16_t value_max = flipper_ecu_map_get_value_max(model->map);
                        if(event->type == InputTypeShort) {
                            if(value < value_max) {
                                flipper_ecu_map_set_value_by_index(
                                    model->map, model->selected_x_dot, value + 1);
                            }
                            consumed = true;
                        } else if(event->type == InputTypeRepeat) {
                            if(value < value_max - 5) {
                                flipper_ecu_map_set_value_by_index(
                                    model->map, model->selected_x_dot, value + 5);
                            } else if(value < value_max) {
                                flipper_ecu_map_set_value_by_index(
                                    model->map, model->selected_x_dot, value + 1);
                            }
                            consumed = true;
                        }
                    } else if(event->key == InputKeyDown) {
                        int16_t value =
                            flipper_ecu_map_get_value_by_index(model->map, model->selected_x_dot);
                        int16_t value_min = flipper_ecu_map_get_value_min(model->map);
                        if(event->type == InputTypeShort) {
                            if(value > value_min) {
                                flipper_ecu_map_set_value_by_index(
                                    model->map, model->selected_x_dot, value - 1);
                            }
                            consumed = true;
                        } else if(event->type == InputTypeRepeat) {
                            if(value > value_min + 5) {
                                flipper_ecu_map_set_value_by_index(
                                    model->map, model->selected_x_dot, value - 5);
                            } else if(value > value_min) {
                                flipper_ecu_map_set_value_by_index(
                                    model->map, model->selected_x_dot, value - 1);
                            }
                            consumed = true;
                        }
                    }
                }
            }
        },
        true);
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
