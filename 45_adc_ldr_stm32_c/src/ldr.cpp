#include "ldr.h"
#include "config.h"

Ldr::Ldr(ADC_HandleTypeDef *hadc) : hadc(hadc)
{
}

uint16_t Ldr::read()
{
    HAL_ADC_Start(hadc);
    HAL_ADC_PollForConversion(hadc, ADC_POLL_TIMEOUT_MILLIS);
    return (uint16_t)HAL_ADC_GetValue(hadc);
}

uint16_t Ldr::read_calibrated()
{
    uint16_t raw = read();
    if (raw > CALIBRATED_LDR_MAX)
    {
        return MAX_VALUE;
    }
    else if (raw < CALIBRATED_LDR_MIN)
    {
        return MIN_VALUE;
    }
    return (uint16_t)((uint32_t)(raw - CALIBRATED_LDR_MIN)) * (MAX_VALUE) / (CALIBRATED_LDR_MAX - CALIBRATED_LDR_MIN);
}

uint16_t Ldr::reverse_value(uint16_t value)
{

    return MAX_VALUE - value;
}
