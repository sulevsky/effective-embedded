#pragma once

#include "driver/gpio.h"

class Switch
{
private:
    const gpio_num_t _gpio_num;

public:
    Switch(gpio_num_t gpio_num);
    void init();
    bool is_on();
};
