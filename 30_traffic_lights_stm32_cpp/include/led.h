#pragma once

#include "stdint.h"
#include "gpio.h"

// supports GPIOA only
class Led
{
private:
    const Gpio gpio;
    // time to take full on/off cycle
    const uint32_t blinking_period_millis;
    bool is_blinking;

public:
    Led(const Gpio gpio, uint32_t blinking_period_millis);
    void init();
    void on();
    void off();
    void blinking();
    void on_timer_event(uint32_t counter_millis);
};
