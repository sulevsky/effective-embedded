#pragma once

#include "driver/gpio.h"

class Button
{
private:
    gpio_num_t _gpio_num;

public:
    Button(gpio_num_t gpio_num);
    void init();
    bool is_pressed();
};