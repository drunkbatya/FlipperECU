#pragma once

#include <furi.h>

#include "workers/adc_worker/flipper_ecu_adc_worker.h"

typedef enum {
    GPIO_ADC_MCU_1_MAP = 0,
    GPIO_ADC_MCU_2_TPS = 1,
    GPIO_ADC_MCU_3_AIR_TEMP = 2,
    GPIO_ADC_MCU_4_WATER_TEMP = 3,
    GPIO_ADC_MCU_5_GP = 4,
    GPIO_ADC_MCU_6_GP = 5,
    GPIO_ADC_MCU_7_VIN = 6,
    GPIO_ADC_MCU_5_5V = 7,
    GPIO_ADC_NONE = 8,
} GpioAdcPins;

typedef double (*Converter)(FlipperECUAdcWorker* worker, double value);

typedef struct {
    const GpioPin* pin;
    const uint32_t channel;
    const uint32_t rank;
    double ext_voltage_div_ratio;
    Converter converter;
} GpioPinAdcRecord;

extern const GpioPinAdcRecord gpio_adc_pins[];
extern const size_t gpio_adc_pins_size;

extern const GpioPin* const gpio_mcu_fuel_pump_out;

extern const GpioPin* const gpio_mcu_idle_step;
extern const GpioPin* const gpio_mcu_idle_direction;

extern const GpioPin* const gpio_mcu_ign_1;
extern const GpioPin* const gpio_mcu_ign_2;

extern const GpioPin* const gpio_mcu_inj_1;
extern const GpioPin* const gpio_mcu_inj_2;
extern const GpioPin* const gpio_mcu_inj_3;
extern const GpioPin* const gpio_mcu_inj_4;
