#include "driver/gpio.h"
#include "config.h"
#include "led.h"

uint32_t Led::to_level(const LedState led_state)
{
    switch (led_state)
    {
    case LedState::OFF:
        return Config::Common::LEVEL_LOW;
    case LedState::ON:
        return Config::Common::LEVEL_HIGH;
    default:
        __builtin_unreachable();
    }
}

Led::Led(const gpio_num_t gpio_num, const LedState initial_led_state) : _gpio_num(gpio_num)
{
    _level = to_level(initial_led_state);
}

void Led::init()
{
    gpio_reset_pin(_gpio_num);
    gpio_set_direction(_gpio_num, GPIO_MODE_OUTPUT);
    gpio_set_level(_gpio_num, _level);
}

void Led::set_state(const LedState led_state)
{
    uint32_t gpio_level = to_level(led_state);

    if (gpio_level != _level)
    {
        _level = gpio_level;
        gpio_set_level(_gpio_num, gpio_level);
    }
}
