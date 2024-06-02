#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <gui/modules/submenu.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/widget.h>
#include "scenes/flipper_ecu_scene.h"

#include "../flipper_ecu_app.h"

typedef struct {
    FlipperECUApp* ecu_app;
    ViewDispatcher* view_dispatcher;
    Gui* gui;
    FuriMessageQueue* event_queue;
    SceneManager* scene_manager;
    Submenu* submenu;
    Widget* widget;
} FlipperECUGui;

typedef enum {
    FlipperECUGuiViewSubmenu,
    FlipperECUGuiViewWidget,
} FlipperECUGuiView;
