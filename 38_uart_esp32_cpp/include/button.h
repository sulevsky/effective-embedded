#pragma once

#include "driver/gpio.h"

class Button
{
private:
    const gpio_num_t _gpio_num;
    volatile bool is_triggered_flag;

public:
    Button(const gpio_num_t gpio_num);
    ~Button();
    void init();
    void set_is_triggered_flag();
    void reset_is_triggered_flag();
    bool is_triggered();
};