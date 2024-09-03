#pragma once

#include <furi.h>

typedef enum {
    GPIO_ADC_MCU_1_MAP = 0,
    GPIO_ADC_MCU_2_TPS = 1,
    GPIO_ADC_MCU_3_AIR_TEMP = 2,
    GPIO_ADC_MCU_4_WATER_TEMP = 3,
    GPIO_ADC_MCU_5_GP = 4,
    GPIO_ADC_MCU_6_GP = 5,
    GPIO_ADC_NONE = 6,
} GpioAdcPins;

typedef struct {
    const GpioPin* pin;
    const uint32_t channel;
    const uint32_t rank;
} GpioPinAdcRecord;

extern const GpioPin* const gpio_adc_mcu_1_map;
extern const GpioPin* const gpio_adc_mcu_2_tps;
extern const GpioPin* const gpio_adc_mcu_3_air_temp;
extern const GpioPin* const gpio_adc_mcu_4_water_temp;
extern const GpioPin* const gpio_adc_mcu_5_gp;
extern const GpioPin* const gpio_adc_mcu_6_gp;

extern const GpioPinAdcRecord gpio_adc_pins[];
extern const size_t gpio_adc_pins_size;
