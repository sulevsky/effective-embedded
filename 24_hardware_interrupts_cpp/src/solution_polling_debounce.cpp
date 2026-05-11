#include "driver/gpio.h"
#include "button.h"
#include "config.h"
#include "time_utils.h"
#include "state_management/state_context.h"

void solution_polling_debounce()
{
    printf("Experiment 4: polling debounce  - started\n");

    printf("Init button, with debounce\n");
    Button button(Config::Button::BUTTON_GPIO);
    button.init(GPIO_INTR_DISABLE);

    uint32_t waiting_start_time_ms = now_millis();
    printf("Press button %ld times, for %ld seconds\n", Config::Experiment::NUMBER_OF_PRESSES_PER_EXPERIMENT, Config::Experiment::EXPERIMENT_TIME_MILLIS / Config::Common::MILLIS_IN_SECONDS);
    printf("Waiting...\n");

    EventGenerator event_generator(&button);
    StateContext state_context(&event_generator);
    while (!is_expired(now_millis(), waiting_start_time_ms, Config::Experiment::EXPERIMENT_TIME_MILLIS))
    {
        state_context.update();
    }

    printf("Number of short presses: %ld\n", state_context.get_short_press_num());
    printf("Number of long presses: %ld\n", state_context.get_long_press_num());
    printf("Experiment 4 - finished\n\n");
}
