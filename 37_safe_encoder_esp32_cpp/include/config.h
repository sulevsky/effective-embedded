#pragma once

#include "driver/gpio.h"

namespace Config
{

    namespace Common
    {
        static const uint8_t CODE_LENGTH = 4;
        static const uint8_t MAX_TRIES = 3;

        static const uint32_t MILLIS_IN_SECONDS = 1000;
        static const uint8_t CODE[4] = {5, 4, 0, 9};
    }

    namespace Button
    {
        static const gpio_num_t BUTTON_GPIO = GPIO_NUM_17;
        static const int ESP_INTR_FLAG_DEFAULT = 0;

        static const uint32_t DEFAULT_DEBOUNCE_DELAY_MICROS = 50000;

        static const uint32_t HELD_LONG_MIN_TIME_MICROS = 2000000;

        static const uint32_t POLL_DELAY_MILLIS = 5;
    };
    namespace Encoder
    {
        static const gpio_num_t ENCODER_GPIO_A = GPIO_NUM_15;
        static const gpio_num_t ENCODER_GPIO_B = GPIO_NUM_16;
    };
    namespace Led
    {
        static const gpio_num_t RED_GPIO = GPIO_NUM_6;
        static const gpio_num_t GREEN_GPIO = GPIO_NUM_5;

        static const uint64_t BLINKING_CHECK_INTERVAL_MICROS = 10000;
    };
}
