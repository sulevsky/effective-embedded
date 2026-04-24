#pragma once

#include "stdint.h"
#include "led.h"
#include "switch.h"

enum class State
{
    Green,
    GreenBlinking,
    Yellow,
    YellowBlinking,
    Red,
    RedYellow,
};

void handle_state_transition(uint32_t now, Led *red, Led *yellow, Led *green, Switch *switcher);
