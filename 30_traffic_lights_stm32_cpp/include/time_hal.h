#pragma once

#include "stdint.h"
#include "stm32f4xx_hal.h"

inline uint32_t now_millis()
{
    return HAL_GetTick();
}

inline void delay(uint32_t millis)
{
    HAL_Delay(millis);
}
