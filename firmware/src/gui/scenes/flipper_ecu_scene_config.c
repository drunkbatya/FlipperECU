#include "../flipper_ecu_gui_i.h"

typedef enum {
    FlipperECUSceneConfigSubmenuIndexCKPS,
} FlipperECUSceneConfigSubmenuIndex;

static void flipper_ecu_scene_config_submenu_callback(void* context, uint32_t index) {
    FlipperECUGui* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void flipper_ecu_scene_config_on_enter(void* context) {
    FlipperECUGui* app = context;
    submenu_add_item(
        app->submenu,
        "CKPS",
        FlipperECUSceneConfigSubmenuIndexCKPS,
        flipper_ecu_scene_config_submenu_callback,
        app);
    submenu_set_selected_item(
        app->submenu, scene_manager_get_scene_state(app->scene_manager, FlipperECUSceneConfig));
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipperECUGuiViewSubmenu);
}

bool flipper_ecu_scene_config_on_event(void* context, SceneManagerEvent event) {
    FlipperECUGui* app = context;
    bool success = false;
    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(app->scene_manager, FlipperECUSceneConfig, event.event);
        if(event.event == FlipperECUSceneConfigSubmenuIndexCKPS) {
            scene_manager_next_scene(app->scene_manager, FlipperECUSceneConfigCKPS);
            success = true;
        }
    }
    return success;
}

void flipper_ecu_scene_config_on_exit(void* context) {
    FlipperECUGui* app = context;
    submenu_reset(app->submenu);
}
