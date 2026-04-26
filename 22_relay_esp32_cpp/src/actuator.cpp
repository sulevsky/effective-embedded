#include "driver/gpio.h"
#include "config.h"
#include "actuator.h"

uint32_t Actuator::to_level(const ActuatorState actuator_state)
{
    switch (actuator_state)
    {
    case ActuatorState::OFF:
        return Config::Common::LEVEL_LOW;
    case ActuatorState::ON:
        return Config::Common::LEVEL_HIGH;
    default:
        __builtin_unreachable();
    }
}

Actuator::Actuator(const gpio_num_t gpio_num, const ActuatorState initial_actuator_state) : _gpio_num(gpio_num)
{
    _level = to_level(initial_actuator_state);
}

void Actuator::init()
{
    gpio_reset_pin(_gpio_num);
    gpio_set_direction(_gpio_num, GPIO_MODE_OUTPUT);
    gpio_set_level(_gpio_num, _level);
}

void Actuator::set_state(const ActuatorState actuator_state)
{
    uint32_t gpio_level = to_level(actuator_state);

    if (gpio_level != _level)
    {
        _level = gpio_level;
        gpio_set_level(_gpio_num, gpio_level);
    }
}
