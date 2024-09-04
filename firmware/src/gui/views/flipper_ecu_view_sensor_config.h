#pragma once

#include <gui/view.h>
#include <stdint.h>

#include "../../flipper_ecu_engine_status.h"

typedef struct FlipperECUSensorConfigView FlipperECUSensorConfigView;

typedef enum {
    SENSOR_TYPE_MAP,
    SENSOR_TYPE_MAF,
    SENSOR_TYPE_TEMP,
    SENSOR_TYPE_TPS,
    SENSOR_TYPE_NONE
} SensorType;

FlipperECUSensorConfigView* flipper_ecu_view_sensor_config_alloc(void);
void flipper_ecu_view_sensor_config_free(FlipperECUSensorConfigView* view_sensor_config);
View* flipper_ecu_view_sensor_config_get_view(FlipperECUSensorConfigView* view_sensor_config);
void flipper_ecu_view_sensor_config_view_update(FlipperECUSensorConfigView* view_sensor_config);
void flipper_ecu_view_sensor_config_set_sensor_type(
    FlipperECUSensorConfigView* view_sensor_config,
    SensorType type);
void flipper_ecu_view_sensor_config_reset(FlipperECUSensorConfigView* view_sensor_config);
void flipper_ecu_view_sensor_config_set_adc_raw_ptr(
    FlipperECUSensorConfigView* view_sensor_config,
    float* ptr);
