#include "switch.h"

Switch::Switch(const Gpio gpio) : gpio(gpio) {}

void Switch::init()
{
    // here GPIOA is hardcoded
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = gpio.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(gpio.port, &GPIO_InitStruct);
}

bool Switch::is_on()
{
    return HAL_GPIO_ReadPin(gpio.port, gpio.pin) == GPIO_PIN_RESET;
}
