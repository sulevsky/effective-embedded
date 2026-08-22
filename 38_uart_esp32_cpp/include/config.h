#pragma once

#include "driver/gpio.h"

namespace Config
{
    namespace Common
    {
        static const uint32_t MILLIS_IN_SECONDS = 1000;

        static const uint32_t LOGGING_UART_INIT_DELAY_MILLIS = 3000;
    }

    namespace Button
    {
        static const gpio_num_t BUTTON_GPIO = GPIO_NUM_4;
        static const int ESP_INTR_FLAG_DEFAULT = 0;
        static const uint32_t DEFAULT_DEBOUNCE_DELAY_MICROS = 200000;
    };
    namespace Led
    {
        static const gpio_num_t REMOTE_LED_GPIO = GPIO_NUM_5;
        static const gpio_num_t LOCAL_LED_GPIO = GPIO_NUM_6;
    };
}
