#include "config.h"
#include "led.h"
#include "gpio.h"

Led::Led(const Gpio gpio, const uint32_t blinking_period_millis) : gpio(gpio), blinking_period_millis(blinking_period_millis)
{
    is_blinking = false;
}

void Led::init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // here GPIOA is hardcoded
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(gpio.port, gpio.pin, GPIO_PIN_RESET);

    /*Configure GPIO pins*/
    GPIO_InitStruct.Pin = gpio.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void Led::on()
{
    is_blinking = false;
    HAL_GPIO_WritePin(gpio.port, gpio.pin, GPIO_PinState::GPIO_PIN_SET);
}

void Led::off()
{
    is_blinking = false;
    HAL_GPIO_WritePin(gpio.port, gpio.pin, GPIO_PinState::GPIO_PIN_RESET);
}

void Led::blinking()
{
    is_blinking = true;
}

void Led::on_timer_event(const uint32_t counter_millis)
{
    if (!is_blinking)
    {
        return;
    }
    uint32_t duration_from_start_of_period = counter_millis % blinking_period_millis;
    uint32_t on_part_of_the_period = blinking_period_millis / 2;
    if (duration_from_start_of_period < on_part_of_the_period)
    {
        HAL_GPIO_WritePin(this->gpio.port, gpio.pin, GPIO_PinState::GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(this->gpio.port, gpio.pin, GPIO_PinState::GPIO_PIN_RESET);
    }
}
