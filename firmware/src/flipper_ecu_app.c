#include <furi.h>

#include "flipper_ecu_app.h"

FlipperECUApp* flipper_ecu_app_alloc(void) {
    FlipperECUApp* app = malloc(sizeof(FlipperECUApp));
    app->settings = flipper_ecu_settings_alloc();
    app->gui = flipper_ecu_gui_alloc(app);

    app->sync_worker = flipper_ecu_sync_worker_alloc(app->settings);
    return app;
}

void flipper_ecu_app_free(FlipperECUApp* app) {
    furi_assert(app);
    flipper_ecu_sync_worker_free(app->sync_worker);
    flipper_ecu_settings_free(app->settings);

    flipper_ecu_gui_free(app->gui);
    free(app);
}

int32_t flipper_ecu_app(void* p) {
    UNUSED(p);

    FlipperECUApp* app = flipper_ecu_app_alloc();
    flipper_ecu_gui_start(app->gui);

    flipper_ecu_gui_await_stop(app->gui);

    flipper_ecu_sync_worker_send_stop(app->sync_worker);

    flipper_ecu_sync_worker_await_stop(app->sync_worker);
    flipper_ecu_app_free(app);
    free(app);
    return 0;
}
