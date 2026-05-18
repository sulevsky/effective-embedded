#include "driver/gpio.h"
#include "config.h"
#include "led.h"

Led::Led(gpio_num_t gpio_num, uint32_t _blinking_period_millis) : _gpio_num(gpio_num), _blinking_period_millis(_blinking_period_millis)
{
    _is_blinking = false;
}

void Led::init()
{
    gpio_reset_pin(_gpio_num);
    gpio_set_direction(_gpio_num, GPIO_MODE_OUTPUT);
    gpio_set_level(_gpio_num, OFF_LEVEL);
}

void Led::on()
{
    _is_blinking = false;
    gpio_set_level(_gpio_num, ON_LEVEL);
}

void Led::off()
{
    _is_blinking = false;
    gpio_set_level(_gpio_num, OFF_LEVEL);
}

void Led::blinking()
{
    _is_blinking = true;
}

void Led::on_timer_event(uint32_t counter_millis)
{
    if (!_is_blinking)
    {
        return;
    }
    uint32_t duration_from_start_of_period = counter_millis % _blinking_period_millis;
    uint32_t on_part_of_the_period = _blinking_period_millis / 2;
    if (duration_from_start_of_period < on_part_of_the_period)
    {
        gpio_set_level(_gpio_num, ON_LEVEL);
    }
    else
    {
        gpio_set_level(_gpio_num, OFF_LEVEL);
    }
}
