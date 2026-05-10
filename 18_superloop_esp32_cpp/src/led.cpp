#include "driver/gpio.h"
#include "config.h"
#include "led.h"
#include "time_utils.h"

uint32_t Led::to_level(const State led_state)
{
    switch (led_state)
    {
    case State::OFF:
        return Config::Common::LEVEL_LOW;
    case State::ON:
        return Config::Common::LEVEL_HIGH;
    default:
        __builtin_unreachable();
    }
}

Led::Led(const gpio_num_t gpio_num, const State initial_led_state, const uint32_t period_millis) : _gpio_num(gpio_num), _period_millis(period_millis)
{
    _level = to_level(initial_led_state);
    _last_toggle = 0;
}

void Led::init()
{
    gpio_reset_pin(_gpio_num);
    gpio_set_direction(_gpio_num, GPIO_MODE_OUTPUT);
    gpio_set_level(_gpio_num, _level);
}

void Led::set_state(const State led_state)
{
    uint32_t gpio_level = to_level(led_state);

    if (gpio_level != _level)
    {
        _level = gpio_level;
        gpio_set_level(_gpio_num, gpio_level);
    }
}
void Led::toggle_if_expired(uint32_t now)
{
    if (!is_expired(now, _last_toggle, _period_millis))
    {
        return;
    }
    _last_toggle  = now;
    uint32_t new_level;
    if (_level == Config::Common::LEVEL_LOW)
    {
        _level = Config::Common::LEVEL_HIGH;
    }
    else if (_level == Config::Common::LEVEL_HIGH)
    {
        _level = Config::Common::LEVEL_LOW;
    }
    else
    {
        __unreachable();
    }
    gpio_set_level(_gpio_num, _level);
}
