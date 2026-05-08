#pragma once

#include "servo_utils.h"

uint32_t reverted_servo_ccr(uint32_t input)
{
    return CCR_MIN + CCR_MAX - input;
}

uint32_t into_servo_ccr(uint32_t input)
{
    if (input < ADC_CALIBRATED_MIN)
    {
        return CCR_MIN;
    }
    if (input > ADC_CALIBRATED_MAX)
    {
        return CCR_MAX;
    }
    return CCR_MIN + CCR_RANGE * (input - ADC_CALIBRATED_MIN) / ADC_RANGE;
}

float into_degrees(uint32_t ccr)
{
    return (((float)ccr - CCR_MIN) * RANGE_DEGREEES) / CCR_RANGE - START_DEGREE;
}
