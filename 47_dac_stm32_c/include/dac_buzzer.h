#pragma once

#include "stdint.h"
#include "stm32f4xx_hal.h"

class DacBuzzer
{
private:
    DAC_HandleTypeDef *hdac;
    const uint32_t channel;
    const uint32_t alignment;

public:
    DacBuzzer(DAC_HandleTypeDef *hdac, const uint32_t channel, const uint32_t alignment);
    void start();
    void set_value(uint16_t value);
    void play(uint16_t frequency, uint32_t duration_millis);
};
