#include "switch.h"

Switch::Switch(gpio_num_t gpio_num) : _gpio_num(gpio_num) {}

void Switch::init()
{
    gpio_reset_pin(_gpio_num);
    gpio_set_direction(_gpio_num, GPIO_MODE_INPUT);
    gpio_set_pull_mode(_gpio_num, GPIO_PULLUP_ONLY);
}

bool Switch::is_on()
{
    return gpio_get_level(_gpio_num) == 0;
}
