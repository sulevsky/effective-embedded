#pragma once

#include "stdint.h"
#include "gpio.h"

class Led
{
private:
    const Gpio gpio;

public:
    Led(const Gpio gpio);
    void set_state(bool is_off);
};
