#pragma once

#include "driver/gpio.h"

class Led
{
private:
    static const uint32_t OFF_LEVEL = 0;
    static const uint32_t ON_LEVEL = 1;
    const gpio_num_t _gpio_num;
    // time to take full on/off cycle
    const uint32_t _blinking_period_millis;
    bool _is_blinking;

public:
    Led(gpio_num_t gpio_num, uint32_t _blinking_period_millis);
    void init();
    void on();
    void off();
    void blinking();
    void on_timer_event(uint32_t counter_millis);
};
