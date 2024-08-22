#include <storage/storage.h>
#include <dialogs/dialogs.h>
#include "flipper_ecu_icons.h"

#include "../flipper_ecu_gui_i.h"
#include "../../flipper_ecu_engine_settings.h"
#include "../../flipper_ecu_files.h"

void flipper_ecu_scene_load_on_enter(void* context) {
    FlipperECUGui* app = context;
    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(
        &browser_options, ENGINE_SETTINGS_FILE_EXT, &I_Dip8_10px);
    browser_options.base_path = STORAGE_APP_DATA_PATH_PREFIX;
    bool success =
        dialog_file_browser_show(app->dialogs, app->file_path, app->file_path, &browser_options);
    if(success) {
        if(flipper_ecu_files_load(app->ecu_app)) {
            scene_manager_next_scene(app->scene_manager, FlipperECUSceneSuccess);
        } else {
            dialog_message_show_storage_error(app->dialogs, "Cannot load\nfile");
            scene_manager_previous_scene(app->scene_manager);
        }
    } else {
        scene_manager_previous_scene(app->scene_manager);
    }
}

bool flipper_ecu_scene_load_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void flipper_ecu_scene_load_on_exit(void* context) {
    UNUSED(context);
}
