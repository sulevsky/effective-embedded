#include "driver/gpio.h"
#include "esp_timer.h"
#include "button.h"
#include "config.h"
#include "time_utils.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_cpu.h"

void solution_interrupt_no_debounce()
{
    printf("Experiment 1: No debounce - started\n");

    printf("Init button, no debounce\n");
    Button button(ButtonConfig::BUTTON_GPIO);
    button.init(GPIO_INTR_NEGEDGE);

    uint32_t waiting_start_time_ms = now_millis();
    printf("Press button %ld times, for %ld seconds\n", ExperimentConfig::NUMBER_OF_PRESSES_PER_EXPERIMENT, ExperimentConfig::EXPERIMENT_TIME_MILLIS / CommonConfig::MILLIS_IN_SECONDS);
    printf("Waiting...\n");
    while (!is_expired(now_millis(), waiting_start_time_ms, ExperimentConfig::EXPERIMENT_TIME_MILLIS))
    {
        asm("nop");
    }

    printf("Number of interrupts: %ld\n", button.get_counter());
    button.~Button();
    printf("Experiment 1: No debounce - finished\n\n");
}
