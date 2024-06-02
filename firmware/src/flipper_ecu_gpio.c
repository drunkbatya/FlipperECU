#include "flipper_ecu_gpio.h"

#include <furi_hal_resources.h>

static const FlipperECUGPIO flipper_ecu_gpio = {.ckp_pin = &gpio_ext_pb2};

const FlipperECUGPIO* flipper_ecu_gpio_init(void) {
    const FlipperECUGPIO* gpio = &flipper_ecu_gpio;

    furi_hal_gpio_init(
        flipper_ecu_gpio.ckp_pin, GpioModeInterruptRiseFall, GpioPullUp, GpioSpeedVeryHigh);

    return gpio;
}

void flipper_ecu_gpio_deinit(void) {
    furi_hal_gpio_init(flipper_ecu_gpio.ckp_pin, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
}
