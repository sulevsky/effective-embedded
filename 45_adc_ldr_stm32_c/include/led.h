#pragma once
#include "stdint.h"

class Led
{
private:
    static const uint32_t FREQUENCY_HZ = 5000;

public:
    Led();
    void set_state(uint16_t state);
};