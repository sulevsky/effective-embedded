#include "driver/gpio.h"
#include "config.h"
#include "led.h"

Led::Led(const gpio_num_t gpio_num) : _gpio_num(gpio_num)
{
}

void Led::init()
{
    gpio_reset_pin(_gpio_num);
    gpio_set_direction(_gpio_num, GPIO_MODE_OUTPUT);
    gpio_set_level(_gpio_num, static_cast<uint32_t>(Level::OFF));
}

void Led::on()
{
    gpio_set_level(_gpio_num, static_cast<uint32_t>(Level::ON));
}

void Led::off()
{
    gpio_set_level(_gpio_num, static_cast<uint32_t>(Level::OFF));
}
