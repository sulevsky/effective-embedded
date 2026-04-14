#pragma once

#include "driver/gpio.h"

class Input
{
private:
    gpio_num_t _gpio_num;

public:
    Input(gpio_num_t gpio_num);
    void init();
    bool is_on();
    uint64_t get_measurement_end();
    void reset_measurement();
};
