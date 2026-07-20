#pragma once

#include "stdint.h"
#include "stm32f4xx_hal.h"

class Adc
{
private:
    static const uint16_t MAX_VALUE = (1 << 12) - 1;
    static const uint16_t MIN_VALUE = 0;
    ADC_HandleTypeDef *hadc;

public:
    Adc(ADC_HandleTypeDef *hadc);
    uint16_t read();
};