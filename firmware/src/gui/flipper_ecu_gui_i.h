#pragma once

#include "flipper_ecu_gui.h"

#include <furi.h>
#include <gui/gui.h>
#include <gui/modules/submenu.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/widget.h>
#include <gui/modules/variable_item_list.h>
#include "scenes/config/flipper_ecu_scene.h"
#include "views/flipper_ecu_view_dashboard.h"
#include "views/flipper_ecu_view_map_editor.h"
#include "../flipper_ecu_app.h"

struct FlipperECUGui {
    FuriThread* thread;
    FlipperECUApp* ecu_app;
    ViewDispatcher* view_dispatcher;
    Gui* gui;
    FuriMessageQueue* event_queue;
    SceneManager* scene_manager;
    Submenu* submenu;
    Widget* widget;
    VariableItemList* var_item_list;
    FlipperECUDashboardView* view_dashboard;
    FlipperECUMapEditorView* view_map_editor;
};

typedef enum {
    FlipperECUGuiViewSubmenu,
    FlipperECUGuiViewWidget,
    FlipperECUGuiViewVarItemList,
    FlipperECUGuiViewDashboard,
    FlipperECUGuiViewMapEditor,
} FlipperECUGuiView;
