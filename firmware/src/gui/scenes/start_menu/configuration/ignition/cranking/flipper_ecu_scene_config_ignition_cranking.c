#include "../../../../../flipper_ecu_gui_i.h"

typedef enum {
    FlipperECUSceneConfigIgnitionCrankingSubmenuIndexMap,
} FlipperECUSceneConfigIgnitionCrankingSubmenuIndex;

static void
    flipper_ecu_scene_config_ignition_cranking_submenu_callback(void* context, uint32_t index) {
    FlipperECUGui* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void flipper_ecu_scene_config_ignition_cranking_on_enter(void* context) {
    FlipperECUGui* app = context;
    submenu_add_item(
        app->submenu,
        "Map [2D]",
        FlipperECUSceneConfigIgnitionCrankingSubmenuIndexMap,
        flipper_ecu_scene_config_ignition_cranking_submenu_callback,
        app);
    submenu_set_selected_item(
        app->submenu,
        scene_manager_get_scene_state(app->scene_manager, FlipperECUSceneConfigIgnitionCranking));
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipperECUGuiViewSubmenu);
}

bool flipper_ecu_scene_config_ignition_cranking_on_event(void* context, SceneManagerEvent event) {
    FlipperECUGui* app = context;
    bool success = false;
    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(
            app->scene_manager, FlipperECUSceneConfigIgnitionCranking, event.event);
        if(event.event == FlipperECUSceneConfigIgnitionCrankingSubmenuIndexMap) {
            scene_manager_next_scene(app->scene_manager, FlipperECUSceneConfigIgnitionCrankingMap);
            success = true;
        }
    }
    return success;
}

void flipper_ecu_scene_config_ignition_cranking_on_exit(void* context) {
    FlipperECUGui* app = context;
    submenu_reset(app->submenu);
}
