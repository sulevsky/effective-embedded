#pragma once

#include "driver/gpio.h"
#include "driver/pulse_cnt.h"

class Encoder
{
private:
    const gpio_num_t gpio_num_a;
    const gpio_num_t gpio_num_b;
    pcnt_unit_handle_t handle;

public:
    Encoder(const gpio_num_t gpio_num_a, const gpio_num_t gpio_num_b);
    void init();
    int32_t get_count();
};
