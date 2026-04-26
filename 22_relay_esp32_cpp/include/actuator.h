#pragma once

#include "driver/gpio.h"

class Actuator
{
public:
    enum class ActuatorState
    {
        OFF,
        ON
    };

private:
    const gpio_num_t _gpio_num;
    uint32_t _level;
    uint32_t to_level(const ActuatorState actuator_state);

public:
    Actuator(const gpio_num_t gpio_num, const ActuatorState actuator_state);
    void init();
    void set_state(const ActuatorState actuator_state);
};
