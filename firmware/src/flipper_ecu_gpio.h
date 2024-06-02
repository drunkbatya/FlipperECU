#pragma once

#include <furi.h>

typedef struct {
    const GpioPin* ckp_pin;
} FlipperECUGPIO;

const FlipperECUGPIO* flipper_ecu_gpio_init(void);
void flipper_ecu_gpio_deinit(void);
