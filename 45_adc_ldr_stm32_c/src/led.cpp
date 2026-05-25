#include "config.h"
#include "led.h"
#include "gpio.h"

Led::Led(const Gpio gpio) : gpio(gpio)
{
}

void Led::set_state(bool is_off)
{
    if (is_off)
    {
        HAL_GPIO_WritePin(gpio.port, gpio.pin, GPIO_PinState::GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(gpio.port, gpio.pin, GPIO_PinState::GPIO_PIN_SET);
    }
}
