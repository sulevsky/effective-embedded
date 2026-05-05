#pragma once

#include "driver/gpio.h"

struct CommonConfig
{
    static const uint32_t MILLIS_IN_SECONDS = 1000;
    static const uint32_t MICROS_IN_MILLIS = 1000;

    static const uint32_t MICROSECOND_RESOLUTION = 1000000;
    static const uint32_t INITIAL_ALARM_COUNT = 10 * MICROS_IN_MILLIS;
};

namespace Config
{
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
