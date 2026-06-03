#pragma once

#include "stdint.h"
#include "gpio.h"

class SquareBuzzer
{
private:
    const Gpio gpio;

public:
    SquareBuzzer(const Gpio gpio);
    void set_state(bool is_on);
    void play(uint16_t frequency, uint32_t duration_millis);
};
