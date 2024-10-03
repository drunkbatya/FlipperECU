#include "../../../../../flipper_ecu_gui_i.h"

#include "../../../../../../flipper_ecu_engine_config.h"

static float test = 1234.123;

void flipper_ecu_scene_config_sensors_tps_on_enter(void* context) {
    FlipperECUGui* app = context;
    flipper_ecu_view_sensor_config_set_sensor_type(app->view_sensor_config, SENSOR_TYPE_TPS);
    flipper_ecu_view_sensor_config_set_adc_raw_ptr(app->view_sensor_config, &test);
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipperECUGuiViewSensorConfig);
}

bool flipper_ecu_scene_config_sensors_tps_on_event(void* context, SceneManagerEvent event) {
    FlipperECUGui* app = context;
    if(event.type == SceneManagerEventTypeTick) {
        flipper_ecu_view_sensor_config_view_update(app->view_sensor_config);
        return true;
    }
    return false;
}

void flipper_ecu_scene_config_sensors_tps_on_exit(void* context) {
    FlipperECUGui* app = context;
    flipper_ecu_view_sensor_config_reset(app->view_sensor_config);
}
