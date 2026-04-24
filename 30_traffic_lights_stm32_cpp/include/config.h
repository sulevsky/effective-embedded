#pragma once

#include "gpio.h"

namespace Config
{
    namespace Common
    {
        static const uint32_t MILLIS_IN_SECONDS = 1000;
        static const uint32_t MICROS_IN_MILLIS = 1000;

        static const uint32_t MICROSECOND_RESOLUTION = 1000000;
        static const uint32_t INITIAL_ALARM_COUNT = 10 * MICROS_IN_MILLIS;

        static const uint64_t TIMER_CHECK_INTERVAL_MILLIS = 10;
    };

    namespace Switch
    {
        static const Gpio SWITCH_GPIO = Gpio{
            GPIOA,
            GPIO_PIN_6,
        };
    };
    namespace Led
    {
        static const Gpio RED_GPIO = Gpio{
            GPIOA,
            GPIO_PIN_3,
        };
        // any>0, not used
        static const uint32_t RED_BLINKING_PERIOD_MILLIS = 2;

        static const Gpio YELLOW_GPIO = Gpio{
            GPIOA,
            GPIO_PIN_4,
        };
        static const uint32_t YELLOW_BLINKING_PERIOD_MILLIS = 1000;

        static const Gpio GREEN_GPIO = Gpio{
            GPIOA,
            GPIO_PIN_5,
        };
        static const uint32_t GREEN_BLINKING_PERIOD_MILLIS = 500;

        static const uint32_t RED_DURATION_MILLIS = 30 * Common::MILLIS_IN_SECONDS;
        static const uint32_t RED_YELLOW_DURATION_MILLIS = 10 * Common::MILLIS_IN_SECONDS;
        static const uint32_t GREEN_DURATION_MILLIS = 30 * Common::MILLIS_IN_SECONDS;
        static const uint32_t GREEN_BLINKING_DURATION_MILLIS = 10 * Common::MILLIS_IN_SECONDS;
        static const uint32_t YELLOW_DURATION_MILLIS = 10 * Common::MILLIS_IN_SECONDS;
        static const uint32_t YELLOW_BLINKING_DURATION_MILLIS = 5 * Common::MILLIS_IN_SECONDS;
    };
}
