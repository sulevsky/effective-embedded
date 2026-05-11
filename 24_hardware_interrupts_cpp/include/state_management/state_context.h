#pragma once

#include "state_management/event_generator.h"

class State;

class StateContext
{
public:
    StateContext(EventGenerator *event_generator);
    uint32_t get_short_press_num();
    uint32_t get_long_press_num();
    void increment_long_press_num();
    void increment_short_press_num();
    void update();
    void start_timer(const uint64_t delay_micros);

private:
    uint32_t short_press_num;
    uint32_t long_press_num;
    State *state;
    EventGenerator *event_generator;
};
