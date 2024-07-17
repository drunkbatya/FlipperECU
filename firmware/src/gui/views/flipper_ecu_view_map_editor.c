#include "flipper_ecu_view_map_editor.h"

#include <furi.h>
#include <gui/elements.h>

#define MAP_NAME_SIZE 20 + 1
#define X_Y_NAME_SIZE 11 + 1

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define FONT_HEIGHT 10

#define EDITOR_ITEM_H 64 / 3
#define EDITOR_ITEM_W 128

struct FlipperECUMapEditorView {
    View* view;
};

typedef enum { ModeView, ModeEdit } Mode;

static int16_t test_data[16] = {0, 40, 13, 12, 20, 15, 16, 17, 18, 29, 28, 27, 10, 9, 13};
static const int16_t test_keys[16] =
    {600, 720, 840, 990, 1170, 1380, 1650, 1950, 2310, 2730, 3210, 3840, 4530, 5370, 6360, 7500};

typedef struct {
    uint8_t map_size;
    uint8_t prev_dot_x;
    uint8_t prev_dot_y;
    uint8_t selected_x_dot;
    Mode mode;
    int16_t val_min;
    int16_t val_max;
    char map_name[MAP_NAME_SIZE];
    char x_name[X_Y_NAME_SIZE];
    char val_name[X_Y_NAME_SIZE];
    int16_t* data;
    const int16_t* keys;
} FlipperECUMapEditorViewModel;

void flipper_ecu_view_map_editor_load_map(FlipperECUMapEditorView* view_map_editor) {
    with_view_model(
        view_map_editor->view,
        FlipperECUMapEditorViewModel * model,
        {
            model->map_size = 16;
            model->val_min = -15;
            model->val_max = 55;
            strncpy(model->map_name, "Ignition main map", MAP_NAME_SIZE - 1);
            strncpy(model->x_name, "> RPM", X_Y_NAME_SIZE - 1);
            strncpy(model->val_name, "^ Angle", X_Y_NAME_SIZE - 1);
            model->data = test_data;
            model->keys = test_keys;
            model->mode = ModeView;
            model->selected_x_dot = 3;
        },
        true);
}

static void flipper_ecu_view_map_editor_draw_scaled_dot(Canvas* canvas, uint8_t x, uint8_t y) {
    canvas_draw_dot(canvas, x, y);
    canvas_draw_dot(canvas, x - 1, y);
    canvas_draw_dot(canvas, x + 1, y);
    canvas_draw_dot(canvas, x, y - 1);
    canvas_draw_dot(canvas, x - 1, y - 1);
    canvas_draw_dot(canvas, x + 1, y - 1);
    canvas_draw_dot(canvas, x, y + 1);
    canvas_draw_dot(canvas, x - 1, y + 1);
    canvas_draw_dot(canvas, x + 1, y + 1);
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
    canvas_draw_str_aligned(canvas, 0, 0, AlignLeft, AlignTop, map_editor_model->val_name);

    // Map title
    canvas_draw_str_aligned(
        canvas, DISPLAY_WIDTH, 0, AlignRight, AlignTop, map_editor_model->map_name);

    uint16_t step = main_field_width / (map_editor_model->map_size - 1);
    uint16_t value_range = map_editor_model->val_max - map_editor_model->val_min;
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
    for(uint8_t x_model = 0; x_model < map_editor_model->map_size; x_model++) {
        int16_t current_value_to_calc = map_editor_model->data[x_model];
        if(map_editor_model->val_min < 0) {
            current_value_to_calc += -(map_editor_model->val_min);
        } else if(map_editor_model->val_min > 0) {
            current_value_to_calc -= map_editor_model->val_min;
        }
        uint16_t x = (x_model * step) + main_field_start_x;
        uint8_t y =
            main_field_height - (current_value_to_calc * y_step / scale) + main_field_start_y - 1;

        // dots and line
        flipper_ecu_view_map_editor_draw_scaled_dot(canvas, x, y);
        if(x_model < map_editor_model->map_size + 1) {
            if(x_model != 0) {
                canvas_draw_line(
                    canvas, map_editor_model->prev_dot_x, map_editor_model->prev_dot_y, x, y);
            }
            map_editor_model->prev_dot_x = x;
            map_editor_model->prev_dot_y = y;
        }

        // editing mode
        if(map_editor_model->mode == ModeEdit) {
            if(x_model == map_editor_model->selected_x_dot) {
                // lines
                flipper_ecu_view_map_editor_draw_dotted_line(
                    canvas, x, y, x, main_field_start_y + main_field_height - 1); // line to x
                flipper_ecu_view_map_editor_draw_dotted_line(
                    canvas, main_field_start_x, y, x, y); // line to y

                // Values
                furi_string_printf(fstr, "key: %d", map_editor_model->keys[x_model]);
                canvas_draw_str_aligned(
                    canvas,
                    0,
                    DISPLAY_HEIGHT - 1,
                    AlignLeft,
                    AlignBottom,
                    furi_string_get_cstr(fstr));
                furi_string_printf(fstr, "value: %d", map_editor_model->data[x_model]);
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
        canvas, DISPLAY_WIDTH, DISPLAY_HEIGHT, AlignRight, AlignBottom, map_editor_model->x_name);
    furi_string_free(fstr);
}

static bool flipper_ecu_view_map_editor_input_callback(InputEvent* event, void* context) {
    FlipperECUMapEditorView* view_map_editor = context;
    bool consumed = false;
    with_view_model(
        view_map_editor->view,
        FlipperECUMapEditorViewModel * model,
        {
            if(model->mode != ModeEdit) {
                if((event->type == InputTypeShort) || (event->type == InputTypeRepeat)) {
                    if(event->key == InputKeyOk) {
                        model->mode = ModeEdit;
                        consumed = true;
                    }
                }
            } else {
                if((event->type == InputTypeShort) || (event->type == InputTypeRepeat)) {
                    if((event->key == InputKeyOk) || (event->key == InputKeyBack)) {
                        if(event->type == InputTypeShort) {
                            model->mode = ModeView;
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
                            if(model->selected_x_dot < model->map_size - 1) {
                                model->selected_x_dot += 1;
                            }
                            consumed = true;
                        } else if(event->type == InputTypeRepeat) {
                            if(model->selected_x_dot < model->map_size - 5 - 1) {
                                model->selected_x_dot += 5;
                            } else if(model->selected_x_dot < model->map_size - 1) {
                                model->selected_x_dot += 1;
                            }
                            consumed = true;
                        }
                    } else if(event->key == InputKeyUp) {
                        if(event->type == InputTypeShort) {
                            if(model->data[model->selected_x_dot] < model->val_max) {
                                model->data[model->selected_x_dot] += 1;
                            }
                            consumed = true;
                        } else if(event->type == InputTypeRepeat) {
                            if(model->data[model->selected_x_dot] < model->val_max - 5) {
                                model->data[model->selected_x_dot] += 5;
                            } else if(model->data[model->selected_x_dot] < model->val_max) {
                                model->data[model->selected_x_dot] += 1;
                            }
                            consumed = true;
                        }
                    } else if(event->key == InputKeyDown) {
                        if(event->type == InputTypeShort) {
                            if(model->data[model->selected_x_dot] > model->val_min) {
                                model->data[model->selected_x_dot] -= 1;
                            }
                            consumed = true;
                        } else if(event->type == InputTypeRepeat) {
                            if(model->data[model->selected_x_dot] > model->val_min + 5) {
                                model->data[model->selected_x_dot] -= 5;
                            } else if(model->data[model->selected_x_dot] > model->val_min) {
                                model->data[model->selected_x_dot] -= 1;
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
