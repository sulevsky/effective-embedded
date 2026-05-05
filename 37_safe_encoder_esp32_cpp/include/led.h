#pragma once

#include "driver/gpio.h"

class Led
{
private:
    enum class Level : uint32_t
    {
        OFF = 0,
        ON = 1,
    };
    const gpio_num_t _gpio_num;

public:
    Led(const gpio_num_t gpio_num);
    void init();
    void on();
    void off();
};
