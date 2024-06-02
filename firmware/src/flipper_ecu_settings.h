#pragma once

#include <furi.h>

typedef struct {
    uint8_t test;
} FlipperECUSettings;

FlipperECUSettings* flipper_ecu_settings_alloc(void);
void flipper_ecu_settings_free(FlipperECUSettings* settings);
