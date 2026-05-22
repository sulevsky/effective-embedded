#pragma once

#include "driver/gpio.h"

class Led
{
private:
    static const uint32_t OFF_LEVEL = 0;
    static const uint32_t ON_LEVEL = 1;
    const gpio_num_t _gpio_num;

public:
    Led(gpio_num_t gpio_num);
    void init();
    void toggle();
};