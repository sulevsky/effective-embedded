#pragma once

#include "driver/gpio.h"

enum LedState
{
    OFF,
    ON
};

class Led
{
private:
    gpio_num_t _gpio_num;
    uint32_t _level;
    uint32_t to_level(LedState led_state);

public:
    Led(gpio_num_t gpio_num, LedState led_state);
    void init();
    void set_state(LedState led_state);
    void toggle();
};