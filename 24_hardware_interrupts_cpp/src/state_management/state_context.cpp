#include "state_management/states.h"
#include "state_management/event_generator.h"

StateContext::StateContext(EventGenerator *event_generator) : event_generator(event_generator)
{
    state = get_initial_state();
    short_press_num = 0;
    long_press_num = 0;
}

void StateContext::update()
{
    Event new_event = event_generator->generate_event();
    State *new_state = state->on_event(new_event, this);
    state = new_state;
}

void StateContext::start_timer(const uint64_t delay_micros)
{
    event_generator->start_timer(delay_micros);
}

uint32_t StateContext::get_short_press_num()
{
    return short_press_num;
}
void StateContext::increment_short_press_num()
{
    short_press_num++;
}
uint32_t StateContext::get_long_press_num()
{
    return long_press_num;
}
void StateContext::increment_long_press_num()
{
    long_press_num++;
}
