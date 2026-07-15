#pragma once

#include "driver/gpio.h"

class Led
{
public:
    enum class State
    {
        OFF,
        ON
    };

private:
    const gpio_num_t _gpio_num;
    const uint32_t _period_millis;
    uint32_t _last_toggle;
    uint32_t _level;
    uint32_t to_level(const State led_state);

public:
    Led(const gpio_num_t gpio_num, const State initial_led_state, const uint32_t period_millis);
    void init();
    void set_state(const State led_state);
    void toggle_if_expired(uint32_t now);
};
