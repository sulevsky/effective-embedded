#pragma once

#include "driver/gpio.h"

struct CommonConfig
{
    static constexpr uint32_t LEVEL_HIGH = 1;
    static constexpr uint32_t LEVEL_LOW = 0;

    static constexpr uint32_t MILLIS_IN_MICROS = 1000;
    static constexpr uint32_t DURATION_LOG_ONCE_PER = 100000;
};

struct GPIOConfig
{
    static constexpr gpio_num_t BLUE_LED_GPIO = GPIO_NUM_4;
    static constexpr gpio_num_t RED_LED_GPIO = GPIO_NUM_5;

    static constexpr gpio_num_t EXTERNAL_BUTTON_GPIO = GPIO_NUM_19;
    static constexpr gpio_num_t BUTTON_BOOT = GPIO_NUM_0;
};
struct BlinkConfig
{
    static constexpr uint32_t FAST_PERIOD = 100;
    static constexpr uint32_t SLOW_PERIOD = 4000;
    static constexpr uint32_t DEBOUNCING_TIME = 200;
};
