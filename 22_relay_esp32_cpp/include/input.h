#pragma once

#include "driver/gpio.h"

class Input
{
private:
    const gpio_num_t _gpio_num;
    volatileuint64_t measurement_end;

public:
    Input(const gpio_num_t gpio_num);
    void init();
    bool is_on();
    uint64_t get_measurement_end();
    void set_measurement_end(const uint64_t end);
    void reset_measurement();
};
