#include "button.h"
#include "time_utils.h"
#include "state_management/event_generator.h"

EventGenerator::EventGenerator(Button *button) : button(button)
{
    last_button_is_pressed = false;

    timer_set = false;
    timeout_timer_started_at_micros = 0;
    timeout_delay = 0;
}

Event EventGenerator::generate_event()
{
    bool current_button_is_pressed = button->is_pressed();
    if (!last_button_is_pressed && current_button_is_pressed)
    {
        last_button_is_pressed = true;
        return Event::PRESS;
    }
    else if (last_button_is_pressed && !current_button_is_pressed)
    {
        last_button_is_pressed = false;
        return Event::RELEASE;
    }
    else if (timer_set && is_expired(now_micros(), timeout_timer_started_at_micros, timeout_delay))
    {
        timer_set = false;
        timeout_timer_started_at_micros = 0;
        timeout_delay = 0;
        return Event::TIMEOUT;
    }
    else
    {
        return Event::NONE;
    }
}

void EventGenerator::start_timer(const uint64_t delay_micros)
{
    timer_set = true;
    timeout_timer_started_at_micros = now_micros();
    timeout_delay = delay_micros;
}
