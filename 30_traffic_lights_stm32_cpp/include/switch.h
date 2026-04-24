#pragma once

#include "gpio.h"

// supports GPIOA only
class Switch
{
private:
    const Gpio gpio;

public:
    Switch(Gpio gpio);
    void init();
    bool is_on();
};
