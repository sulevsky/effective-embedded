#include "config.h"
#include "state_management/states.h"

namespace States
{
    static IdleState idle;
    static PressedState pressed;
    static HeldShortState held_short;
    static HeldLongState held_long;
    static ReleasedState released;
}

State *IdleState::on_event(Event event, StateContext *context)
{
    if (event == Event::PRESS)
    {
        context->start_timer(Config::Button::DEFAULT_DEBOUNCE_DELAY_MICROS);
        return &States::pressed;
    }
    return this;
}
State *PressedState::on_event(Event event, StateContext *context)
{
    if (event == Event::TIMEOUT)
    {
        context->start_timer(Config::Button::HELD_LONG_MIN_TIME_MICROS);
        return &States::held_short;
    }
    return this;
}
State *HeldShortState::on_event(Event event, StateContext *context)
{

    if (event == Event::RELEASE)
    {
        context->increment_short_press_num();
        context->start_timer(Config::Button::DEFAULT_DEBOUNCE_DELAY_MICROS);
        return &States::released;
    }
    if (event == Event::TIMEOUT)
    {
        return &States::held_long;
    }
    return this;
}
State *HeldLongState::on_event(Event event, StateContext *context)
{

    if (event == Event::RELEASE)
    {
        context->increment_long_press_num();
        context->start_timer(Config::Button::DEFAULT_DEBOUNCE_DELAY_MICROS);
        return &States::released;
    }

    return this;
}
State *ReleasedState::on_event(Event event, StateContext *context)
{
    if (event == Event::TIMEOUT)
    {
        return &States::idle;
    }

    return this;
}

State *get_initial_state() { return &States::idle; }
