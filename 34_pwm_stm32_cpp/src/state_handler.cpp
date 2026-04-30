#include "config.h"
#include "led.h"
#include "gpio.h"

#include "state_handler.h"
#include "config.h"
#include "time_utils.h"
#include "switch.h"
#include "led.h"

State next_state = State::Red;
uint32_t last_transition = 0;
uint32_t state_duration = 0;

void handle_state_transition(const uint32_t now, Led *red, Led *yellow, Led *green, Switch *switcher)
{
    if (!is_expired(now, last_transition, state_duration))
    {
        return;
    }
    last_transition = now;
    switch (next_state)
    {
    case State::Green:
        red->off();
        yellow->off();
        green->on();

        next_state = State::GreenBlinking;
        state_duration = Config::Led::GREEN_DURATION_MILLIS;
        break;
    case State::GreenBlinking:
        red->off();
        yellow->off();
        green->blinking();

        next_state = State::Yellow;
        state_duration = Config::Led::GREEN_BLINKING_DURATION_MILLIS;
        break;
    case State::Yellow:
        red->off();
        yellow->on();
        green->off();

        next_state = State::Red;
        state_duration = Config::Led::YELLOW_DURATION_MILLIS;
        break;
    case State::YellowBlinking:
        red->off();
        yellow->blinking();
        green->off();

        if (switcher->is_on())
        {
            state_duration = Config::Led::YELLOW_BLINKING_DURATION_MILLIS;
            next_state = State::YellowBlinking;
        }
        else
        {
            state_duration = Config::Led::YELLOW_BLINKING_DURATION_MILLIS;
            next_state = State::Red;
        }
        break;
    case State::Red:
        red->on();
        yellow->off();
        green->off();

        if (switcher->is_on())
        {
            state_duration = Config::Led::RED_DURATION_MILLIS;
            next_state = State::YellowBlinking;
        }
        else
        {
            state_duration = Config::Led::RED_DURATION_MILLIS;
            next_state = State::RedYellow;
        }
        break;
    case State::RedYellow:
        red->on();
        yellow->on();
        green->off();

        state_duration = Config::Led::RED_YELLOW_DURATION_MILLIS;
        next_state = State::Green;
        break;

    default:
        break;
    }
}
