#pragma once

#include "driver/gpio.h"

class Button
{
private:
    const gpio_num_t _gpio_num;

public:
    Button(const gpio_num_t gpio_num);
    ~Button();
    void init(const gpio_int_type_t intr_type);
    uint32_t get_counter();
    void reset();
    bool is_triggered();
    void reset_triggered_flag();
    bool is_pressed();
};
