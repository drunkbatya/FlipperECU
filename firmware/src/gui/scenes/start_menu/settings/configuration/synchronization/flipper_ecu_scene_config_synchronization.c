#include "../../../../../flipper_ecu_gui_i.h"

#include "../../../../../../flipper_ecu_engine_config.h"

static const char* ckps_polarity[] =
    {[CKPSPolatityRasing] = "Rasing", [CKPSPolatityFalling] = "Falling"};

static void ckps_polarity_cb(VariableItem* item) {
    uint8_t index = variable_item_get_current_value_index(item);
    FlipperECUGui* app = variable_item_get_context(item);
    FlipperECUSyncWorker* sync_worker = flipper_ecu_app_get_sync_worker(app->ecu_app);
    const FlipperECUEngineConfig* engine_config = flipper_ecu_sync_worker_get_config(sync_worker);
    FlipperECUEngineConfig engine_config_new;
    memcpy(&engine_config_new, engine_config, sizeof(FlipperECUEngineConfig));
    engine_config_new.ckps_polarity = index;
    flipper_ecu_sync_worker_update_config_restart(sync_worker, &engine_config_new);

    variable_item_set_current_value_text(item, ckps_polarity[index]);
}

void flipper_ecu_scene_config_sync_on_enter(void* context) {
    FlipperECUGui* app = context;
    VariableItem* item;

    FlipperECUSyncWorker* sync_worker = flipper_ecu_app_get_sync_worker(app->ecu_app);
    const FlipperECUEngineConfig* engine_config = flipper_ecu_sync_worker_get_config(sync_worker);

    item = variable_item_list_add(
        app->var_item_list, "CKPS polarity", COUNT_OF(ckps_polarity), ckps_polarity_cb, app);
    variable_item_set_current_value_index(item, engine_config->ckps_polarity);
    variable_item_set_current_value_text(item, ckps_polarity[engine_config->ckps_polarity]);

    variable_item_list_set_selected_item(
        app->var_item_list,
        scene_manager_get_scene_state(app->scene_manager, FlipperECUSceneConfigSync));

    view_dispatcher_switch_to_view(app->view_dispatcher, FlipperECUGuiViewVarItemList);
}

bool flipper_ecu_scene_config_sync_on_event(void* context, SceneManagerEvent event) {
    FlipperECUGui* app = context;
    UNUSED(app);
    UNUSED(event);
    bool success = false;
    return success;
}

void flipper_ecu_scene_config_sync_on_exit(void* context) {
    FlipperECUGui* app = context;
    scene_manager_set_scene_state(
        app->scene_manager,
        FlipperECUSceneConfigSync,
        variable_item_list_get_selected_item_index(app->var_item_list));
    variable_item_list_reset(app->var_item_list);
}
