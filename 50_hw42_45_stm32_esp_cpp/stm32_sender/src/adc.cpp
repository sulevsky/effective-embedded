#include "adc.h"
#include "config.h"

Adc::Adc(ADC_HandleTypeDef *hadc) : hadc(hadc)
{
}

uint16_t Adc::read()
{
    HAL_ADC_Start(hadc);
    HAL_ADC_PollForConversion(hadc, AdcConfig::ADC_POLL_TIMEOUT_MILLIS);
    return (uint16_t)HAL_ADC_GetValue(hadc);
}
