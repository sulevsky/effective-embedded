#pragma once

#include "driver/gpio.h"

enum class LedState
{
    OFF,
    ON,
};

class Led
{
private:
    const gpio_num_t _gpio_num;
    uint32_t _level;
    uint32_t to_level(const LedState led_state);

public:
    Led(const gpio_num_t gpio_num, const LedState led_state);
    void init();
    void set_state(const LedState led_state);
};
