#include <furi.h>

#include "flipper_ecu_app_i.h"

static FlipperECUApp* flipper_ecu_app_alloc(void) {
    FlipperECUApp* app = malloc(sizeof(FlipperECUApp));

    app->engine_settings = flipper_ecu_engine_settings_alloc();
    flipper_ecu_engine_settings_load_d(app->engine_settings);

    app->sync_worker =
        flipper_ecu_sync_worker_alloc(app, &app->engine_status, app->engine_settings);
    app->adc_worker = flipper_ecu_adc_worker_alloc(app);
    app->fuel_pump_worker = flipper_ecu_fuel_pump_worker_alloc(&app->engine_status);
    app->idle_valve_worker =
        flipper_ecu_idle_valve_worker_alloc(app, app->engine_settings, &app->engine_status);

    // need to alloc gui thread in main thread for proper event loop work
    // because of a lot of `furi_check(instance->thread_id == furi_thread_get_current_id());`
    // in `flipperzero-firmware/furi/core/event_loop.c`
    app->gui_thread = furi_thread_alloc_ex("FlipperECUGui", 2048, flipper_ecu_gui_thread, app);
    return app;
}

static void flipper_ecu_app_free(FlipperECUApp* app) {
    furi_assert(app);
    flipper_ecu_sync_worker_free(app->sync_worker);
    flipper_ecu_adc_worker_free(app->adc_worker);
    flipper_ecu_fuel_pump_worker_free(app->fuel_pump_worker);
    flipper_ecu_idle_valve_worker_free(app->idle_valve_worker);

    flipper_ecu_engine_settings_free(app->engine_settings);

    furi_thread_free(app->gui_thread);
    free(app);
}

int32_t flipper_ecu_app(void* p) {
    UNUSED(p);

    FlipperECUApp* app = flipper_ecu_app_alloc();

    furi_thread_start(app->gui_thread);
    flipper_ecu_adc_worker_start(app->adc_worker);
    // sync worker must be started after adc worker's first measurement
    while(!flipper_ecu_adc_worker_first_measurement_done(app->adc_worker));
    ;
    flipper_ecu_fuel_pump_worker_start(app->fuel_pump_worker);
    furi_delay_tick(10);
    flipper_ecu_sync_worker_start(app->sync_worker);
    flipper_ecu_idle_valve_worker_start(app->idle_valve_worker);

    furi_thread_join(app->gui_thread);

    flipper_ecu_sync_worker_send_stop(app->sync_worker);
    flipper_ecu_sync_worker_await_stop(app->sync_worker);
    flipper_ecu_adc_worker_send_stop(app->adc_worker);
    flipper_ecu_adc_worker_await_stop(app->adc_worker);
    flipper_ecu_fuel_pump_worker_send_stop(app->fuel_pump_worker);
    flipper_ecu_fuel_pump_worker_await_stop(app->fuel_pump_worker);
    flipper_ecu_idle_valve_worker_send_stop(app->idle_valve_worker);
    flipper_ecu_idle_valve_worker_await_stop(app->idle_valve_worker);

    flipper_ecu_app_free(app);
    return 0;
}

FlipperECUSyncWorker* flipper_ecu_app_get_sync_worker(FlipperECUApp* app) {
    return app->sync_worker;
}
FlipperECUFuelPumpWorker* flipper_ecu_app_get_fuel_pump_worker(FlipperECUApp* app) {
    return app->fuel_pump_worker;
}
FlipperECUAdcWorker* flipper_ecu_app_get_adc_worker(FlipperECUApp* app) {
    return app->adc_worker;
}
FlipperECUIdleValveWorker* flipper_ecu_app_get_idle_valve_worker(FlipperECUApp* app) {
    return app->idle_valve_worker;
}
FlipperECUEngineSettings* flipper_ecu_app_get_engine_settings(FlipperECUApp* app) {
    return app->engine_settings;
}
