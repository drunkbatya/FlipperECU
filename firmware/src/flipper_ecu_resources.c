#include "flipper_ecu_resources.h"

#include <furi_hal_resources.h>
#include <stm32wbxx_ll_adc.h>

const GpioPin* const gpio_adc_mcu_1_map = &gpio_ext_pc0;
const GpioPin* const gpio_adc_mcu_2_tps = &gpio_ext_pc1;
const GpioPin* const gpio_adc_mcu_3_air_temp = &gpio_ext_pc3;
const GpioPin* const gpio_adc_mcu_4_water_temp = &gpio_ext_pa0;
const GpioPin* const gpio_adc_mcu_5_gp = &gpio_ext_pa1;
const GpioPin* const gpio_adc_mcu_6_gp = &gpio_ext_pa2;

const GpioPinAdcRecord gpio_adc_pins[] = {
    {.pin = gpio_adc_mcu_1_map, .channel = LL_ADC_CHANNEL_1},
    {.pin = gpio_adc_mcu_2_tps, .channel = LL_ADC_CHANNEL_2},
    {.pin = gpio_adc_mcu_3_air_temp, .channel = LL_ADC_CHANNEL_4},
    {.pin = gpio_adc_mcu_4_water_temp, .channel = LL_ADC_CHANNEL_5},
    {.pin = gpio_adc_mcu_5_gp, .channel = LL_ADC_CHANNEL_5},
    {.pin = gpio_adc_mcu_6_gp, .channel = LL_ADC_CHANNEL_6},
};

const size_t gpio_adc_pins_size = COUNT_OF(gpio_adc_pins);
