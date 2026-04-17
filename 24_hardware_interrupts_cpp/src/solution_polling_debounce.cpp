#include "driver/gpio.h"
#include "esp_timer.h"
#include "button.h"
#include "config.h"
#include "time_utils.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_cpu.h"

typedef enum
{
    PRESS,
    RELEASE,
    TIMEOUT,
    NONE,
} event_t;

typedef enum
{
    STATE_IDLE,
    STATE_PRESSED,
    STATE_HELD_SHORT,
    STATE_HELD_LONG,
    STATE_RELEASED,
} state_t;

static bool timer_set = false;
static uint64_t timeout_timer_started_at_micros = 0;
static uint64_t timeout_delay = 0;

static bool last_button_is_pressed = false;

void start_timer(uint64_t delay_micros)
{
    timer_set = true;
    timeout_timer_started_at_micros = now_micros();
    timeout_delay = delay_micros;
}
event_t get_latest_event(Button *button)
{
    bool current_button_is_pressed = button->is_pressed();
    if (!last_button_is_pressed && current_button_is_pressed)
    {
        last_button_is_pressed = true;
        return PRESS;
    }
    else if (last_button_is_pressed && !current_button_is_pressed)
    {
        last_button_is_pressed = false;
        return RELEASE;
    }
    else if (timer_set && is_expired(now_micros(), timeout_timer_started_at_micros, timeout_delay))
    {
        timer_set = false;
        timeout_timer_started_at_micros = 0;
        timeout_delay = 0;
        return TIMEOUT;
    }
    else
    {
        return NONE;
    }
}

void solution_polling_debounce()
{
    printf("Experiment 4: polling debounce  - started\n");

    printf("Init button, with debounce\n");
    Button button(ButtonConfig::BUTTON_GPIO);
    button.init(GPIO_INTR_DISABLE);

    uint32_t short_press_num = 0;
    uint32_t long_press_num = 0;

    uint32_t waiting_start_time_ms = now_millis();
    printf("Press button %ld times, for %ld seconds\n", ExperimentConfig::NUMBER_OF_PRESSES_PER_EXPERIMENT, ExperimentConfig::EXPERIMENT_TIME_MILLIS / CommonConfig::MILLIS_IN_SECONDS);
    printf("Waiting...\n");

    state_t current_state = STATE_IDLE;
    while (!is_expired(now_millis(), waiting_start_time_ms, ExperimentConfig::EXPERIMENT_TIME_MILLIS))
    {
        event_t event = get_latest_event(&button);
        switch (current_state)
        {
        case STATE_IDLE:
            switch (event)
            {
            case PRESS:
                current_state = STATE_PRESSED;
                start_timer(ButtonConfig::DEFAULT_DEBOUNCE_DELAY_MICROS);
                break;
            case RELEASE:
                /* empty */
                break;
            case TIMEOUT:
                /* empty */
                break;
            case NONE:
                /* empty */
                break;
            }
            break;
        case STATE_PRESSED:
            switch (event)
            {
            case PRESS:
                /* empty */
                break;
            case RELEASE:
                /* empty */
                break;
            case TIMEOUT:
                current_state = STATE_HELD_SHORT;
                short_press_num++;
                start_timer(ButtonConfig::HELD_LONG_MIN_TIME_MICROS);
                break;
            case NONE:
                /* empty */
                break;
            }
            break;
        case STATE_HELD_SHORT:
            switch (event)
            {
            case PRESS:
                /* empty */
                break;
            case RELEASE:
                current_state = STATE_RELEASED;
                start_timer(ButtonConfig::DEFAULT_DEBOUNCE_DELAY_MICROS);
                break;
            case TIMEOUT:
                current_state = STATE_HELD_LONG;
                long_press_num++;
                break;
            case NONE:
                /* empty */
                break;
            }
            break;
        case STATE_HELD_LONG:
            switch (event)
            {
            case PRESS:
                /* empty */
                break;
            case RELEASE:
                current_state = STATE_RELEASED;
                start_timer(ButtonConfig::DEFAULT_DEBOUNCE_DELAY_MICROS);
                break;
            case TIMEOUT:
                /* empty */
                break;
            case NONE:
                /* empty */
                break;
            }
            break;
        case STATE_RELEASED:
            switch (event)
            {
            case PRESS:
                /* empty */
                break;
            case RELEASE:
                /* empty*/
                break;
            case TIMEOUT:
                current_state = STATE_IDLE;
                break;
            case NONE:
                /* empty */
                break;
            }
            break;
        }

        // delay(ButtonConfig::POLL_DELAY_MILLIS);
    }

    printf("Number of short presses: %ld\n", short_press_num);
    printf("Number of long presses: %ld\n", long_press_num);
    button.~Button();
    printf("Experiment 4 - finished\n\n");
}
