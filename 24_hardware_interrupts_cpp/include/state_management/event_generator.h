#pragma once

#include "button.h"

enum class Event
{
    PRESS,
    RELEASE,
    TIMEOUT,
    NONE,
};

class EventGenerator
{
private:
    Button *button;
    bool last_button_is_pressed;

    bool timer_set;
    uint64_t timeout_timer_started_at_micros;
    uint64_t timeout_delay;

public:
    EventGenerator(Button *button);
    Event generate_event();
    void start_timer(const uint64_t delay_micros);
};
