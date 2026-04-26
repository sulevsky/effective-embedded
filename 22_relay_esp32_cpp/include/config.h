#pragma once

#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"

namespace Config
{
    namespace Common
    {
        static const uint32_t MILLIS_IN_MICROS = 1000;
        static const uint32_t LOG_PERIOD_MS = 1000;

        static const uint32_t LEVEL_HIGH = 1;
        static const uint32_t LEVEL_LOW = 0;
        static const uint32_t MEASUREMENTS_NUM = 10;
        static const uint32_t DELAY_BETWEEN_MEASUREMENTS_MILLIS = 1000;
        static const uint32_t DELAY_BETWEEN_MEASUREMENTS_BATCH_MILLIS = 10000;
    };

    namespace Input
    {
        static const gpio_num_t INPUT_GPIO = GPIO_NUM_5;
        static const int ESP_INTR_FLAG_DEFAULT = 0;
    };

    namespace Actuator
    {
        static const gpio_num_t ACTUATOR_GPIO = GPIO_NUM_4;
    };
}
