#pragma once

#include "driver/gpio.h"

class Button
{
private:
    const gpio_num_t _gpio_num;
    volatile uint32_t counter;
    volatile bool is_triggered_flag;

public:
    Button(const gpio_num_t gpio_num);
    ~Button();
    void init(const gpio_int_type_t intr_type);
    uint32_t get_counter();
    void increment_counter();
    void set_is_triggered_flag();
    void reset_is_triggered_flag();
    void reset();
    bool is_triggered();
    bool is_pressed();
};
