#pragma once

#include "driver/gpio.h"

struct CommonConfig
{
    static const uint32_t MILLIS_IN_SECONDS = 1000;
    static const uint32_t MICROS_IN_MILLIS = 1000;

    static const uint32_t MICROSECOND_RESOLUTION = 1000000;
    static const uint32_t INITIAL_ALARM_COUNT = 10 * MICROS_IN_MILLIS;
};

struct SwitchConfig
{
    static const gpio_num_t SWITCH_GPIO = GPIO_NUM_7;
};
struct LedConfig
{
    static const gpio_num_t RED_GPIO = GPIO_NUM_6;
    // any>0, not used
    static const uint32_t RED_BLINKING_PERIOD_MILLIS = 2;

    static const gpio_num_t YELLOW_GPIO = GPIO_NUM_5;
    static const uint32_t YELLOW_BLINKING_PERIOD_MILLIS = 1000;

    static const gpio_num_t GREEN_GPIO = GPIO_NUM_4;
    static const uint32_t GREEN_BLINKING_PERIOD_MILLIS = 500;

    static const uint32_t RED_DURATION_SECONDS = 30;
    static const uint32_t RED_YELLOW_DURATION_SECONDS = 10;
    static const uint32_t GREEN_DURATION_SECONDS = 30;
    static const uint32_t GREEN_BLINKING_DURATION_SECONDS = 10;
    static const uint32_t YELLOW_DURATION_SECONDS = 10;
    static const uint32_t YELLOW_BLINKING_DURATION_SECONDS = 5;

    static const uint64_t BLINKING_CHECK_INTERVAL_MICROS = 10000;
};
