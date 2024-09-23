#include "flipper_ecu_resources.h"

#include <furi_hal_resources.h>
#include <stm32wbxx_ll_adc.h>

//static ouble flipper_ecu_resources_convert

// adc pins
static const GpioPin* const gpio_adc_mcu_1_map = &gpio_ext_pc0;
static const GpioPin* const gpio_adc_mcu_2_tps = &gpio_ext_pc1;
static const GpioPin* const gpio_adc_mcu_3_air_temp = &gpio_ext_pc3;
static const GpioPin* const gpio_adc_mcu_4_water_temp = &gpio_ext_pa0;
static const GpioPin* const gpio_adc_mcu_5_gp = &gpio_ext_pa1;
static const GpioPin* const gpio_adc_mcu_6_gp = &gpio_ext_pa2;
static const GpioPin* const gpio_adc_mcu_7_vin = &gpio_ext_pc5;
static const GpioPin* const gpio_adc_mcu_8_5v = &gpio_ext_pa5;

// pins
const GpioPin* const gpio_mcu_fuel_pump_out = &gpio_ext_pb13;
const GpioPin* const gpio_mcu_fan_out = &gpio_ext_pb9;

// idle driver pins
const GpioPin* const gpio_mcu_idle_direction = &gpio_ibutton; // pb14
const GpioPin* const gpio_mcu_idle_step = &gpio_ext_pa4;

// ignition pins

// injectors pins
const GpioPin* const gpio_mcu_inj_1 = &gpio_ext_pa6;
const GpioPin* const gpio_mcu_inj_2 = &gpio_ext_pb3;
const GpioPin* const gpio_mcu_inj_3 = &gpio_ext_pb2;
const GpioPin* const gpio_mcu_inj_4 = &gpio_ext_pa15;

// Flipper MCU ADC setted up for measuring 0-2048 mV, and
// there is two types of PCB mounted op-amp buffered voltage dividers
// for 0-5V has ratio 1/0.4, for 0-18V has ratio 1/0.109,
// so 5120mv will give us 2048mv for first type
// and 18788mv for second type will give us 2048mv in ADC registers
const GpioPinAdcRecord gpio_adc_pins[] = {
    {.pin = gpio_adc_mcu_1_map,
     .channel = LL_ADC_CHANNEL_1,
     .rank = LL_ADC_REG_RANK_1,
     .ext_voltage_div_ratio = 0.4},
    {.pin = gpio_adc_mcu_2_tps,
     .channel = LL_ADC_CHANNEL_2,
     .rank = LL_ADC_REG_RANK_2,
     .ext_voltage_div_ratio = 0.4},
    {.pin = gpio_adc_mcu_3_air_temp,
     .channel = LL_ADC_CHANNEL_4,
     .rank = LL_ADC_REG_RANK_3,
     .ext_voltage_div_ratio = 0.4},
    {.pin = gpio_adc_mcu_4_water_temp,
     .channel = LL_ADC_CHANNEL_5,
     .rank = LL_ADC_REG_RANK_4,
     .ext_voltage_div_ratio = 0.4},
    {.pin = gpio_adc_mcu_5_gp,
     .channel = LL_ADC_CHANNEL_5,
     .rank = LL_ADC_REG_RANK_5,
     .ext_voltage_div_ratio = 0.4},
    {.pin = gpio_adc_mcu_6_gp,
     .channel = LL_ADC_CHANNEL_6,
     .rank = LL_ADC_REG_RANK_6,
     .ext_voltage_div_ratio = 0.4},
    {.pin = gpio_adc_mcu_7_vin,
     .channel = LL_ADC_CHANNEL_14,
     .rank = LL_ADC_REG_RANK_7,
     .ext_voltage_div_ratio = 0.109},
    {.pin = gpio_adc_mcu_8_5v,
     .channel = LL_ADC_CHANNEL_10,
     .rank = LL_ADC_REG_RANK_8,
     .ext_voltage_div_ratio = 0.4},
};

const size_t gpio_adc_pins_size = COUNT_OF(gpio_adc_pins);
