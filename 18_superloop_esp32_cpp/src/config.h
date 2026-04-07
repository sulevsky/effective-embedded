#pragma once

#include "driver/gpio.h"

struct CommonConfig
{
    static constexpr uint32_t LEVEL_HIGH = 1;
    static constexpr uint32_t LEVEL_LOW = 0;

    static constexpr uint32_t MILLIS_IN_MICROS = 1000;
};

struct GPIOConfig
{
    static constexpr gpio_num_t BLUE_LED_GPIO = GPIO_NUM_4;
    static constexpr gpio_num_t RED_LED_GPIO = GPIO_NUM_5;
    static constexpr gpio_num_t GREEN_LED_GPIO = GPIO_NUM_6;
};
struct BlinkConfig
{
    static constexpr uint32_t BLUE_PERIOD_MS = 200;
    static constexpr uint32_t RED_PERIOD_MS = 500;
    static constexpr uint32_t GREEN_PERIOD_MS = 1000;
};
