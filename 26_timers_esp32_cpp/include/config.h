#pragma once

#include "driver/gpio.h"

struct CommonConfig
{
    static const uint32_t LEVEL_LOW = 0;
    static const uint32_t LEVEL_HIGH = 1;

    static const uint32_t MINUTE_IN_HOUR = 60;
    static const uint32_t  MILLIS_IN_SECONDS = 1000;
    // static const uint32_t MINUTE_PERIOD = 1000000 * 60;
    // this is second timer period for the experiment
    static const uint32_t MINUTE_PERIOD = 1000000;
};

struct SwitchConfig
{
    static const gpio_num_t SWITCH_GPIO = GPIO_NUM_5;
    static const uint32_t SWITCH_IS_ON_MINUTES = 15;
};
