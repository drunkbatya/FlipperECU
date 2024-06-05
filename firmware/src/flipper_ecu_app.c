#include <furi.h>

#include "flipper_ecu_app.h"

FlipperECUApp* flipper_ecu_app_alloc(void) {
    FlipperECUApp* app = malloc(sizeof(FlipperECUApp));
    app->gpio = flipper_ecu_gpio_init();
    app->settings = flipper_ecu_settings_alloc();
    app->gui_thread = furi_thread_alloc_ex("FlipperECUGui", 1024, flipper_ecu_gui_thread, app);
    app->sync_worker = flipper_ecu_sync_worker_alloc(app->settings, app->gpio);
    return app;
}

void flipper_ecu_app_free(FlipperECUApp* app) {
    furi_assert(app);
    furi_thread_free(app->gui_thread);
    flipper_ecu_sync_worker_free(app->sync_worker);
    flipper_ecu_settings_free(app->settings);
    flipper_ecu_gpio_deinit();
    free(app);
}

int32_t flipper_ecu_app(void* p) {
    UNUSED(p);

    FlipperECUApp* app = flipper_ecu_app_alloc();
    furi_thread_start(app->gui_thread);
    flipper_ecu_sync_worker_start(app->sync_worker);

    furi_thread_join(app->gui_thread);

    flipper_ecu_sync_worker_send_stop(app->sync_worker);

    flipper_ecu_sync_worker_await_stop(app->sync_worker);
    flipper_ecu_app_free(app);
    free(app);
    return 0;
}
