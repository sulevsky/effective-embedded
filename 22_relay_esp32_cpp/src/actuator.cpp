#include "driver/gpio.h"
#include "config.h"
#include "actuator.h"

uint32_t Actuator::to_level(ActuatorState actuator_state)
{
    switch (actuator_state)
    {
    case OFF:
        return CommonConfig::LEVEL_HIGH;
    case ON:
        return CommonConfig::LEVEL_LOW;
    default:
        __builtin_unreachable();
    }
}

Actuator::Actuator(gpio_num_t gpio_num, ActuatorState initial_actuator_state)
{
    _gpio_num = gpio_num;
    _level = to_level(initial_actuator_state);
}
void Actuator::init()
{
    gpio_reset_pin(_gpio_num);
    gpio_set_direction(_gpio_num, GPIO_MODE_OUTPUT);
    gpio_set_level(_gpio_num, _level);
}
void Actuator::set_state(ActuatorState actuator_state)
{
    uint32_t gpio_level = to_level(actuator_state);

    if (gpio_level != _level)
    {
        _level = gpio_level;
        gpio_set_level(_gpio_num, gpio_level);
    }
}
