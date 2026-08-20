#pragma once

#include "stdint.h"
#include "stm32f4xx_hal.h"

inline uint32_t now_millis()
{
    return HAL_GetTick();
}

inline uint32_t now_micros()
{
    uint32_t ms, ticks;
    do
    {
        ms = HAL_GetTick();
        ticks = SysTick->LOAD - SysTick->VAL;
    } while (ms != HAL_GetTick());

    return ms * 1000 + ticks / (SystemCoreClock / 1000000);
}

inline void delay(const uint32_t millis)
{
    HAL_Delay(millis);
}
