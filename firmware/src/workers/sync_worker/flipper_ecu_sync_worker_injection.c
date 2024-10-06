#include "flipper_ecu_sync_worker_injection.h"

#include "flipper_ecu_sync_worker_i.h"

double flipper_ecu_sync_worker_injection_get_afterstart_enrichment_multiplyer(
    FlipperECUSyncWorker* worker) {
    FlipperECUAdcWorker* adc_worker = flipper_ecu_app_get_adc_worker(worker->ecu_app);
    const double water_temp = flipper_ecu_adc_worker_get_value_water_temp_full(adc_worker);
    double enrichment_multiplyer = 0;
    if(worker->afterstart_enrichment_counter > 0) {
        double fade_out_value = (double)worker->afterstart_enrichment_counter /
                                (double)worker->engine_settings->afterstart_enrichment_rotations;
        enrichment_multiplyer =
            flipper_ecu_map_interpolate_2d(
                worker->engine_settings->maps[INJ_AFTERSTART_ENRICHMENT], water_temp) /
            (double)1000 * fade_out_value;
        worker->afterstart_enrichment_counter--;
    }
    worker->engine_status->afterstart_enrichment_counter = worker->afterstart_enrichment_counter;
    worker->engine_status->afterstart_enrichment_multiplyer = (double)1.0 + enrichment_multiplyer;
    return worker->engine_status->afterstart_enrichment_multiplyer;
}

double flipper_ecu_sync_worker_injection_get_warmup_enrichment_multiplyer(
    FlipperECUSyncWorker* worker) {
    FlipperECUAdcWorker* adc_worker = flipper_ecu_app_get_adc_worker(worker->ecu_app);
    const double water_temp = flipper_ecu_adc_worker_get_value_water_temp_full(adc_worker);
    double warmup_multiplyer =
        flipper_ecu_map_interpolate_2d(
            worker->engine_settings->maps[INJ_WARMUP_ENRICHMENT], water_temp) /
        (double)1000;
    worker->engine_status->warmup_enrichment_multiplyer = (double)1.0 + warmup_multiplyer;
    return worker->engine_status->warmup_enrichment_multiplyer;
}
