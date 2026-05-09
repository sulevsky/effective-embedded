#pragma once

#include "driver/gpio.h"

namespace Config
{
    namespace Common
    {
        static const uint8_t CODE_LENGTH = 4;
        static const uint8_t MAX_TRIES = 3;

        static const uint32_t MILLIS_IN_SECONDS = 1000;
        static const uint8_t CODE[CODE_LENGTH] = {5, 4, 0, 9};
        
        static const uint32_t UART_INIT_DELAY_MILLIS = 3000;
        static const uint32_t BLOCKED_DELAY_MILLIS = 5000;
        static const uint32_t OPENED_DELAY_MILLIS = 5000;
        
        static const uint32_t ITERATION_DELAY_MILLIS = 10;
    }

    namespace Button
    {
        static const gpio_num_t BUTTON_GPIO = GPIO_NUM_4;
        static const int ESP_INTR_FLAG_DEFAULT = 0;
        static const uint32_t DEFAULT_DEBOUNCE_DELAY_MICROS = 100000;
    };
    namespace Led
    {
        static const gpio_num_t LED_GPIO = GPIO_NUM_5;
    };
    namespace Encoder
    {
        static const gpio_num_t ENCODER_GPIO_A = GPIO_NUM_15;
        static const gpio_num_t ENCODER_GPIO_B = GPIO_NUM_16;
        static const int32_t LOW_LIMIT = -1000;
        static const int32_t HIGH_LIMIT = 1000;
        static const int32_t CYCLE_LENGTH = 4;
    };
}
