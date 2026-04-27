#include "driver/gpio.h"
#include "esp_timer.h"
#include "button.h"
#include "config.h"
#include "time_utils.h"
#include "esp_cpu.h"

enum class Event
{
    PRESS,
    RELEASE,
    TIMEOUT,
    NONE,
};

enum class State
{
    IDLE,
    PRESSED,
    HELD_SHORT,
    HELD_LONG,
    RELEASED,
};

static bool timer_set = false;
static uint64_t timeout_timer_started_at_micros = 0;
static uint64_t timeout_delay = 0;

static bool last_button_is_pressed = false;

void start_timer(const uint64_t delay_micros)
{
    timer_set = true;
    timeout_timer_started_at_micros = now_micros();
    timeout_delay = delay_micros;
}

Event get_latest_event(Button *button)
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

void solution_polling_debounce()
{
    printf("Experiment 4: polling debounce  - started\n");

    printf("Init button, with debounce\n");
    Button button(Config::Button::BUTTON_GPIO);
    button.init(GPIO_INTR_DISABLE);

    uint32_t short_press_num = 0;
    uint32_t long_press_num = 0;

    uint32_t waiting_start_time_ms = now_millis();
    printf("Press button %ld times, for %ld seconds\n", Config::Experiment::NUMBER_OF_PRESSES_PER_EXPERIMENT, Config::Experiment::EXPERIMENT_TIME_MILLIS / Config::Common::MILLIS_IN_SECONDS);
    printf("Waiting...\n");

    State current_state = State::IDLE;
    while (!is_expired(now_millis(), waiting_start_time_ms, Config::Experiment::EXPERIMENT_TIME_MILLIS))
    {
        Event event = get_latest_event(&button);
        switch (current_state)
        {
        case State::IDLE:
            switch (event)
            {
            case Event::PRESS:
                current_state = State::PRESSED;
                start_timer(Config::Button::DEFAULT_DEBOUNCE_DELAY_MICROS);
                break;
            case Event::RELEASE:
                /* empty */
                break;
            case Event::TIMEOUT:
                /* empty */
                break;
            case Event::NONE:
                /* empty */
                break;
            }
            break;
        case State::PRESSED:
            switch (event)
            {
            case Event::PRESS:
                /* empty */
                break;
            case Event::RELEASE:
                /* empty */
                break;
            case Event::TIMEOUT:
                current_state = State::HELD_SHORT;

                start_timer(Config::Button::HELD_LONG_MIN_TIME_MICROS);
                break;
            case Event::NONE:
                /* empty */
                break;
            }
            break;
        case State::HELD_SHORT:
            switch (event)
            {
            case Event::PRESS:
                /* empty */
                break;
            case Event::RELEASE:
                current_state = State::RELEASED;
                short_press_num++;
                start_timer(Config::Button::DEFAULT_DEBOUNCE_DELAY_MICROS);
                break;
            case Event::TIMEOUT:
                current_state = State::HELD_LONG;
                break;
            case Event::NONE:
                /* empty */
                break;
            }
            break;
        case State::HELD_LONG:
            switch (event)
            {
            case Event::PRESS:
                /* empty */
                break;
            case Event::RELEASE:
                current_state = State::RELEASED;
                long_press_num++;
                start_timer(Config::Button::DEFAULT_DEBOUNCE_DELAY_MICROS);
                break;
            case Event::TIMEOUT:
                /* empty */
                break;
            case Event::NONE:
                /* empty */
                break;
            }
            break;
        case State::RELEASED:
            switch (event)
            {
            case Event::PRESS:
                /* empty */
                break;
            case Event::RELEASE:
                /* empty*/
                break;
            case Event::TIMEOUT:
                current_state = State::IDLE;
                break;
            case Event::NONE:
                /* empty */
                break;
            }
            break;
        }
        // No need for delay
        // delay(ButtonConfig::POLL_DELAY_MILLIS);
    }

    printf("Number of short presses: %ld\n", short_press_num);
    printf("Number of long presses: %ld\n", long_press_num);
    button.~Button();
    printf("Experiment 4 - finished\n\n");
}
