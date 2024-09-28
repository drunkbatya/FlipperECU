#pragma once

#include "flipper_ecu_gui.h"

#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/widget.h>
#include <gui/modules/popup.h>
#include <gui/modules/text_input.h>
#include <gui/modules/variable_item_list.h>
#include <dialogs/dialogs.h>
#include "scenes/config/flipper_ecu_scene.h"
#include "views/flipper_ecu_view_dashboard.h"
#include "views/flipper_ecu_view_map_editor.h"
#include "views/flipper_ecu_view_sensor_config.h"
#include "views/flipper_ecu_view_idle_manual.h"

#define FLIPPER_ECU_TEXT_BUFFER_SIZE 100

struct FlipperECUGui {
    FuriThread* thread;
    FlipperECUApp* ecu_app;
    ViewDispatcher* view_dispatcher;
    Gui* gui;
    FuriMessageQueue* event_queue;
    SceneManager* scene_manager;
    Submenu* submenu;
    Widget* widget;
    Popup* popup;
    DialogsApp* dialogs;
    FuriString* file_path;
    TextInput* text_input;
    char text_buffer[FLIPPER_ECU_TEXT_BUFFER_SIZE + 1];
    VariableItemList* var_item_list;
    FlipperECUDashboardView* view_dashboard;
    FlipperECUMapEditorView* view_map_editor;
    FlipperECUSensorConfigView* view_sensor_config;
    FlipperECUIdleManualView* view_idle_manual;
};

typedef enum {
    FlipperECUGuiViewSubmenu,
    FlipperECUGuiViewWidget,
    FlipperECUGuiViewVarItemList,
    FlipperECUGuiViewPopup,
    FlipperECUGuiViewTextInput,
    FlipperECUGuiViewDashboard,
    FlipperECUGuiViewMapEditor,
    FlipperECUGuiViewSensorConfig,
    FlipperECUGuiViewIdleManual,
} FlipperECUGuiView;

typedef enum {
    FlipperECUGuiCustomEventPopupBack,
    FlipperECUGuiCustomEventExitWarningExit,
    FlipperECUGuiCustomEventExitWarningStay,
    FlipperECUGuiCustomEventTextEditResult
} FlipperECUGUICustomEvent;
