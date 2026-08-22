#include "driver/gpio.h"
#include "config.h"
#include "led.h"

Led::Led(gpio_num_t gpio_num) : _gpio_num(gpio_num)
{
}

void Led::init()
{
    gpio_reset_pin(_gpio_num);
    gpio_set_direction(_gpio_num, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_level(_gpio_num, OFF_LEVEL);
}

void Led::toggle()
{
    gpio_set_level(_gpio_num, !gpio_get_level(_gpio_num));
}
