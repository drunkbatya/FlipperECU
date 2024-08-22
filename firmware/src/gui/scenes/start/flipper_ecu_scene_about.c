#include "../flipper_ecu_gui_i.h"

#define VERSION_APP "0.0.1"
#define DEVELOPER "DrunkBatya"
#define GITHUB "https://github.com/drunkbatya/FlipperECU"
#define NAME "\e#\e!       Flipper ECU        \e!\n"
#define BLANK_INV "\e#\e!                                                      \e!\n"

void flipper_ecu_scene_about_on_enter(void* context) {
    FlipperECUGui* app = context;
    FuriString* tmp_string = furi_string_alloc();
    widget_add_text_box_element(
        app->widget, 0, 0, 128, 14, AlignCenter, AlignBottom, BLANK_INV, false);
    widget_add_text_box_element(app->widget, 0, 2, 128, 14, AlignCenter, AlignBottom, NAME, false);
    furi_string_printf(tmp_string, "\e#%s\n", "Information");
    furi_string_cat_printf(tmp_string, "Version: %s\n", VERSION_APP);
    furi_string_cat_printf(tmp_string, "Developed by: %s\n", DEVELOPER);
    furi_string_cat_printf(tmp_string, "Github: %s\n\n", GITHUB);
    furi_string_cat_printf(tmp_string, "\e#%s\n", "Description");
    furi_string_cat_printf(tmp_string, "Flipper-based ECU\n");
    widget_add_text_scroll_element(app->widget, 0, 16, 128, 50, furi_string_get_cstr(tmp_string));

    furi_string_free(tmp_string);
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipperECUGuiViewWidget);
}

bool flipper_ecu_scene_about_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void flipper_ecu_scene_about_on_exit(void* context) {
    FlipperECUGui* app = context;
    widget_reset(app->widget);
}
