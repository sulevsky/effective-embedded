#pragma once

#include "driver/gpio.h"

class Button
{
private:
    const gpio_num_t _gpio_num;

public:
    Button(const gpio_num_t gpio_num);
    void init();
    bool is_pressed();
};
