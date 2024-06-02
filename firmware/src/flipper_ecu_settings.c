#include "flipper_ecu_settings.h"
#include <furi_hal_resources.h>

FlipperECUSettings* flipper_ecu_settings_alloc(void) {
    FlipperECUSettings* settings = malloc(sizeof(FlipperECUSettings));
    return settings;
}

void flipper_ecu_settings_free(FlipperECUSettings* settings) {
    free(settings);
}
