#include "driver/gpio.h"
#include "esp_timer.h"
#include "button.h"
#include "config.h"
#include "time_utils.h"
#include "esp_cpu.h"

void solution_interrupt_debounce_time()
{
    printf("Experiment 2: time debounce in interrupt  - started\n");

    printf("Init button, with debounce\n");
    Button button(Config::Button::BUTTON_GPIO);
    button.init(GPIO_INTR_NEGEDGE);
    uint32_t counter = 0;
    uint64_t last_registered_time = 0;

    uint32_t waiting_start_time_ms = now_millis();
    printf("Press button %ld times, for %ld seconds\n", Config::Experiment::NUMBER_OF_PRESSES_PER_EXPERIMENT, Config::Experiment::EXPERIMENT_TIME_MILLIS / Config::Common::MILLIS_IN_SECONDS);
    printf("Waiting...\n");
    while (!is_expired(now_millis(), waiting_start_time_ms, Config::Experiment::EXPERIMENT_TIME_MILLIS))
    {
        if (button.is_triggered())
        {
            auto now = now_micros();
            if (is_expired(now, last_registered_time, Config::Button::DEFAULT_DEBOUNCE_DELAY_MICROS))
            {
                counter += 1;
                last_registered_time = now;
            }
            button.reset_is_triggered_flag();
        }
    }

    printf("Number of interrupts: %ld\n", counter);
    button.~Button();
    printf("Experiment 2 - finished\n\n");
}
