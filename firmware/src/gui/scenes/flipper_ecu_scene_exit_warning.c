#include <storage/storage.h>
#include <dialogs/dialogs.h>
#include "flipper_ecu_icons.h"

#include "../flipper_ecu_gui_i.h"
#include "../../flipper_ecu_engine_settings.h"
#include "../../flipper_ecu_files.h"

void flipper_ecu_scene_exit_warning_callback(GuiButtonType result, InputType type, void* context) {
    furi_assert(context);
    FlipperECUGui* app = context;

    if((result == GuiButtonTypeRight) && (type == InputTypeShort)) {
        view_dispatcher_send_custom_event(
            app->view_dispatcher, FlipperECUGuiCustomEventExitWarningStay);
    } else if((result == GuiButtonTypeLeft) && (type == InputTypeShort)) {
        view_dispatcher_send_custom_event(
            app->view_dispatcher, FlipperECUGuiCustomEventExitWarningExit);
    }
}

void flipper_ecu_scene_exit_warning_on_enter(void* context) {
    FlipperECUGui* app = context;

    widget_add_text_box_element(
        app->widget,
        0,
        0,
        128,
        54,
        AlignCenter,
        AlignTop,
        "\e#U really wanna exit?\e#\nTurn off ignition before!",
        false);
    widget_add_button_element(
        app->widget, GuiButtonTypeRight, "Stay", flipper_ecu_scene_exit_warning_callback, app);
    widget_add_button_element(
        app->widget, GuiButtonTypeLeft, "Exit", flipper_ecu_scene_exit_warning_callback, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, FlipperECUGuiViewWidget);
}

bool flipper_ecu_scene_exit_warning_on_event(void* context, SceneManagerEvent event) {
    FlipperECUGui* app = context;
    if(event.type == SceneManagerEventTypeBack) {
    } else if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == FlipperECUGuiCustomEventExitWarningStay) {
            scene_manager_previous_scene(app->scene_manager);
            return true;
        } else if(event.event == FlipperECUGuiCustomEventExitWarningExit) {
            scene_manager_stop(app->scene_manager);
            view_dispatcher_stop(app->view_dispatcher);
            return true;
        }
    }
    return false;
}

void flipper_ecu_scene_exit_warning_on_exit(void* context) {
    FlipperECUGui* app = context;
    widget_reset(app->widget);
}
