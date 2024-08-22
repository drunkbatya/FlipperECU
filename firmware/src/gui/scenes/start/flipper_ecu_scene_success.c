#include <storage/storage.h>
#include <dialogs/dialogs.h>
#include "flipper_ecu_icons.h"

#include "../flipper_ecu_gui_i.h"
#include "../../flipper_ecu_engine_settings.h"
#include "../../flipper_ecu_files.h"

static void flipper_ecu_scene_success_popup_callback(void* context) {
    FlipperECUGui* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, FlipperECUGuiCustomEventPopupBack);
}

void flipper_ecu_scene_success_on_enter(void* context) {
    FlipperECUGui* app = context;
    popup_set_icon(app->popup, 32, 5, &I_DolphinNice_96x59);
    popup_set_header(app->popup, "Success!", 5, 7, AlignLeft, AlignTop);
    popup_set_callback(app->popup, flipper_ecu_scene_success_popup_callback);
    popup_set_context(app->popup, app);
    popup_set_timeout(app->popup, 1500);
    popup_enable_timeout(app->popup);
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipperECUGuiViewPopup);
}

bool flipper_ecu_scene_success_on_event(void* context, SceneManagerEvent event) {
    FlipperECUGui* app = context;
    bool success = false;
    if(event.type == SceneManagerEventTypeCustom) {
        success = true;
        if(event.event == FlipperECUGuiCustomEventPopupBack) {
            scene_manager_search_and_switch_to_previous_scene(
                app->scene_manager, FlipperECUSceneSettings);
        }
    }
    return success;
}

void flipper_ecu_scene_success_on_exit(void* context) {
    FlipperECUGui* app = context;
    popup_reset(app->popup);
}
