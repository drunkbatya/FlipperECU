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
double flipper_ecu_adc_worker_get_value_maf(FlipperECUAdcWorker* worker);
