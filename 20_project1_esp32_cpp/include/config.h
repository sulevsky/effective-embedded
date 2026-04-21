#pragma once

#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"

struct CommonConfig
{
    static constexpr uint32_t MILLIS_IN_MICROS = 1000;
    static constexpr uint32_t LOG_PERIOD_MS = 1000;
};

struct LedConfig
{
    static constexpr gpio_num_t LED_GPIO = GPIO_NUM_5;
};

struct LdrConfig
{
    static constexpr adc_unit_t LDR_ADC_UNIT = ADC_UNIT_1;
    static constexpr adc_channel_t LDR_ADC_CHAN = ADC_CHANNEL_7;

    static constexpr uint32_t READ_PERIOD_MS = 20;
};
