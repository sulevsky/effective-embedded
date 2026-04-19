#include "driver/gpio.h"
#include "config.h"

void set_switch(gpio_num_t gpio, bool is_on)
{
    uint32_t level;
    if (is_on)
    {
        level = CommonConfig::LEVEL_HIGH;
    }
    else
    {
        level = CommonConfig::LEVEL_LOW;
    }
    gpio_set_level(gpio, level);
}

void init_switch(gpio_num_t gpio)
{
    gpio_reset_pin(gpio);
    gpio_set_direction(gpio, GPIO_MODE_OUTPUT);
    gpio_set_level(gpio, CommonConfig::LEVEL_HIGH);
}
