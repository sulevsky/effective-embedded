#pragma once

#include "driver/gpio.h"

enum ActuatorState
{
    OFF,
    ON
};

class Actuator
{
private:
    gpio_num_t _gpio_num;
    uint32_t _level;
    uint32_t to_level(ActuatorState actuator_state);

public:
    Actuator(gpio_num_t gpio_num, ActuatorState actuator_state);
    void init();
    void set_state(ActuatorState actuator_state);
};
