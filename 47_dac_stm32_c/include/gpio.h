#pragma once
#include "stm32f4xx_hal.h"

typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;
} Gpio;

// TODO vova extract
typedef struct
{
    const char *name;
    uint16_t frequency;
} Note;
