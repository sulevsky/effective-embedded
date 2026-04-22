#pragma once

#include "driver/gpio.h"

namespace Config
{
    namespace Common
    {
        static const uint32_t LEVEL_HIGH = 1;
        static const uint32_t LEVEL_LOW = 0;

        static const uint32_t MILLIS_IN_MICROS = 1000;
        static const uint32_t DURATION_LOG_ONCE_PER = 100000;
    };

    namespace GPIO
    {
        static const gpio_num_t BLUE_LED_GPIO = GPIO_NUM_4;
        static const gpio_num_t RED_LED_GPIO = GPIO_NUM_5;

        static const gpio_num_t EXTERNAL_BUTTON_GPIO = GPIO_NUM_19;
        static const gpio_num_t BUTTON_BOOT = GPIO_NUM_0;
    };
    namespace Blink
    {
        static const uint32_t FAST_PERIOD = 100;
        static const uint32_t SLOW_PERIOD = 4000;
        static const uint32_t DEBOUNCING_TIME = 200;
    };
}
