#include "driver/gpio.h"
#include "esp_timer.h"
#include "button.h"
#include "config.h"
#include "time_utils.h"
#include "esp_cpu.h"

static bool last_button_is_pressed = false;

void solution_interrupt_debounce_state()
{
    printf("IN PROGRESS\n");
    printf("Experiment 3: state debounce in interrupt  - started\n");

    printf("Init button, with debounce\n");
    Button button(Config::Button::BUTTON_GPIO);
    button.init(GPIO_INTR_ANYEDGE);
    uint32_t counter = 0;

    uint32_t waiting_start_time_ms = now_millis();
    printf("Press button %ld times, for %ld seconds\n", Config::Experiment::NUMBER_OF_PRESSES_PER_EXPERIMENT, Config::Experiment::EXPERIMENT_TIME_MILLIS / Config::Common::MILLIS_IN_SECONDS);
    printf("Waiting...\n");
    while (!is_expired(now_millis(), waiting_start_time_ms, Config::Experiment::EXPERIMENT_TIME_MILLIS))
    {
        if (button.is_triggered())
        {
            bool current_button_is_pressed = button.is_pressed();
            if (!last_button_is_pressed && current_button_is_pressed)
            {
                last_button_is_pressed = true;
                counter += 1;
            }
            else if (last_button_is_pressed && !current_button_is_pressed)
            {
                last_button_is_pressed = false;
            }
            button.reset_is_triggered_flag();
        }
    }

    printf("Number of presses: %ld\n", counter);
    button.~Button();
    printf("Experiment 3 - finished\n\n");
}
