#include "../../flipper_ecu_gui_i.h"

typedef enum {
    FlipperECUSceneStartSubmenuIndexDashboard,
    FlipperECUSceneStartSubmenuIndexSettings,
    FlipperECUSceneStartSubmenuIndexManual,
    FlipperECUSceneStartSubmenuIndexAbout
} FlipperECUSceneStartSubmenuIndex;

static void flipper_ecu_scene_start_submenu_callback(void* context, uint32_t index) {
    FlipperECUGui* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void flipper_ecu_scene_start_on_enter(void* context) {
    FlipperECUGui* app = context;
    submenu_add_item(
        app->submenu,
        "Dashboard",
        FlipperECUSceneStartSubmenuIndexDashboard,
        flipper_ecu_scene_start_submenu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Settings",
        FlipperECUSceneStartSubmenuIndexSettings,
        flipper_ecu_scene_start_submenu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Manual control",
        FlipperECUSceneStartSubmenuIndexManual,
        flipper_ecu_scene_start_submenu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "About",
        FlipperECUSceneStartSubmenuIndexAbout,
        flipper_ecu_scene_start_submenu_callback,
        app);
    submenu_set_selected_item(
        app->submenu, scene_manager_get_scene_state(app->scene_manager, FlipperECUSceneStart));
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipperECUGuiViewSubmenu);
}

bool flipper_ecu_scene_start_on_event(void* context, SceneManagerEvent event) {
    FlipperECUGui* app = context;
    bool success = false;
    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(app->scene_manager, FlipperECUSceneStart, event.event);
        if(event.event == FlipperECUSceneStartSubmenuIndexSettings) {
            scene_manager_next_scene(app->scene_manager, FlipperECUSceneSettings);
            success = true;
        } else if(event.event == FlipperECUSceneStartSubmenuIndexAbout) {
            scene_manager_next_scene(app->scene_manager, FlipperECUSceneAbout);
            success = true;
        } else if(event.event == FlipperECUSceneStartSubmenuIndexDashboard) {
            scene_manager_next_scene(app->scene_manager, FlipperECUSceneDashboard);
            success = true;
        } else if(event.event == FlipperECUSceneStartSubmenuIndexManual) {
            scene_manager_next_scene(app->scene_manager, FlipperECUSceneManual);
            success = true;
        }
    }
    return success;
}

void flipper_ecu_scene_start_on_exit(void* context) {
    FlipperECUGui* app = context;
    submenu_reset(app->submenu);
}
