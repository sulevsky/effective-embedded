#include "driver/gpio.h"
#include "esp_timer.h"
#include "input.h"
#include "actuator.h"
#include "config.h"
#include "time_utils.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_cpu.h"

uint64_t measure_delay(Actuator *actuator, Input *input)
{
    actuator->set_state(OFF);
    uint64_t start = now_micros();
    actuator->set_state(ON);
    while (!input->is_on())
    {
    }

    int64_t end = now_micros();
    actuator->set_state(OFF);
    return end - start;
}

void solution_interrupt()
{
    printf("Init...\n");
    Input input(InputConfig::INPUT_GPIO);
    input.init();

    Actuator actuator(ActuatorConfig::ACTUATOR_GPIO, ActuatorState::OFF);
    actuator.init();
    // delay(2000);
    while (true)
    {
        printf("Starting measurement\n");
        uint64_t time_measurements_sum = 0;
        for (uint32_t i = 0; i < CommonConfig::MEASUREMENTS_NUM; i++)
        {
            input.reset_measurement();
            uint64_t start = now_micros();
            actuator.set_state(ON);
            esp_cpu_wait_for_intr(); //works ok but fails on first invocation
            while (input.get_measurement_end() == 0)
            {
            }

            uint64_t duration = input.get_measurement_end() - start;
            printf("Iteration: %ld, delay: %lld micros\n", i, duration);
            time_measurements_sum += duration;
            actuator.set_state(OFF);
            delay(CommonConfig::DELAY_BETWEEN_MEASUREMENTS_MILLIS);
        }
        printf("Measurements finished, made %ld measurements\n", CommonConfig::MEASUREMENTS_NUM);
        uint64_t mean = time_measurements_sum / CommonConfig::MEASUREMENTS_NUM;
        printf("Mean delay is %lld microseconds\n", mean);
        printf("Next measurement starts in %ld ms\n", CommonConfig::DELAY_BETWEEN_MEASUREMENTS_BATCH_MILLIS);
        delay(CommonConfig::DELAY_BETWEEN_MEASUREMENTS_BATCH_MILLIS);
    }
}

void solution_blocking()
{
    printf("Init...\n");
    Input input(InputConfig::INPUT_GPIO);
    input.init();

    Actuator actuator(ActuatorConfig::ACTUATOR_GPIO, ActuatorState::OFF);
    actuator.init();
    while (true)
    {
        printf("Starting measurement\n");
        uint64_t time_measurements_sum = 0;
        for (uint32_t i = 0; i < CommonConfig::MEASUREMENTS_NUM; i++)
        {
            uint32_t result = measure_delay(&actuator, &input);
            delay(CommonConfig::DELAY_BETWEEN_MEASUREMENTS_MILLIS);
            time_measurements_sum += result;
        }
        printf("Measurements finished, made %ld measurements\n", CommonConfig::MEASUREMENTS_NUM);
        uint64_t mean = time_measurements_sum / CommonConfig::MEASUREMENTS_NUM;
        printf("Mean delay is %lld microseconds\n", mean);
        printf("Next measurement starts in %ld ms\n", CommonConfig::DELAY_BETWEEN_MEASUREMENTS_BATCH_MILLIS);
        delay(CommonConfig::DELAY_BETWEEN_MEASUREMENTS_BATCH_MILLIS);
    }
}

extern "C" void app_main(void)
{
#ifdef SOLUTION_BLOCKING
    solution_blocking();
#else
    solution_interrupt();
#endif
}
