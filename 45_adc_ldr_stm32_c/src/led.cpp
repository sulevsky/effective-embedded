#include "config.h"
#include "led.h"
#include "stdint.h"
#include <cassert>
#include "stm32f4xx_hal.h"
Led::Led() {};

/**
 * 0-2^12 range
 */
void Led::set_state(uint16_t state)
{
    assert(state <= 1 << 12);
    TIM1->CCR2 = state;
    // TODO vova some dummy
    // TODO vova reorg all the project to cpp
}
