#pragma once

typedef struct FlipperECUAdcWorker FlipperECUAdcWorker;

#include <furi.h>
#include "../../flipper_ecu_app.h"

FlipperECUAdcWorker* flipper_ecu_adc_worker_alloc(FlipperECUApp* ecu_app);
void flipper_ecu_adc_worker_start(FlipperECUAdcWorker* worker);
void flipper_ecu_adc_worker_free(FlipperECUAdcWorker* worker);
void flipper_ecu_adc_worker_send_stop(FlipperECUAdcWorker* worker);
void flipper_ecu_adc_worker_await_stop(FlipperECUAdcWorker* worker);
bool flipper_ecu_adc_worker_first_measurement_done(FlipperECUAdcWorker* worker);

double flipper_ecu_adc_worker_get_value_tps(FlipperECUAdcWorker* worker);
double flipper_ecu_adc_worker_get_value_tps_full(FlipperECUAdcWorker* worker);
double flipper_ecu_adc_worker_get_value_map(FlipperECUAdcWorker* worker);
double flipper_ecu_adc_worker_get_value_map_full(FlipperECUAdcWorker* worker);
double flipper_ecu_adc_worker_get_value_vbat(FlipperECUAdcWorker* worker);
double flipper_ecu_adc_worker_get_value_v5v(FlipperECUAdcWorker* worker);
double flipper_ecu_adc_worker_get_value_air_temp(FlipperECUAdcWorker* worker);
double flipper_ecu_adc_worker_get_value_air_temp_full(FlipperECUAdcWorker* worker);
double flipper_ecu_adc_worker_get_value_water_temp(FlipperECUAdcWorker* worker);
double flipper_ecu_adc_worker_get_value_water_temp_full(FlipperECUAdcWorker* worker);

double flipper_ecu_adc_worker_converter_basic(FlipperECUAdcWorker* worker, double input);
double flipper_ecu_adc_worker_converter_tps(FlipperECUAdcWorker* worker, double input);
double flipper_ecu_adc_worker_converter_air_temp(FlipperECUAdcWorker* worker, double input);
double flipper_ecu_adc_worker_converter_water_temp(FlipperECUAdcWorker* worker, double input);
double flipper_ecu_adc_worker_converter_map(FlipperECUAdcWorker* worker, double input);
