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
    actuator->set_state(Actuator::ActuatorState::OFF);
    uint64_t start = now_micros();
    actuator->set_state(Actuator::ActuatorState::ON);
    while (!input->is_on())
    {
    }

    int64_t end = now_micros();
    actuator->set_state(Actuator::ActuatorState::OFF);
    return end - start;
}

void solution_interrupt()
{
    printf("Init...\n");
    Input input(Config::Input::INPUT_GPIO);
    input.init();

    Actuator actuator(Config::Actuator::ACTUATOR_GPIO, Actuator::ActuatorState::OFF);
    actuator.init();
    while (true)
    {
        printf("Starting measurement\n");
        uint64_t time_measurements_sum = 0;
        for (uint32_t i = 0; i < Config::Common::MEASUREMENTS_NUM; i++)
        {
            input.reset_measurement();
            uint64_t start = now_micros();
            actuator.set_state(Actuator::ActuatorState::ON);
            esp_cpu_wait_for_intr(); // works ok but fails on first invocation
            while (input.get_measurement_end() == 0)
            {
            }

            uint64_t duration = input.get_measurement_end() - start;
            printf("Iteration: %ld, delay: %lld micros\n", i, duration);
            time_measurements_sum += duration;
            actuator.set_state(Actuator::ActuatorState::OFF);
            delay(Config::Common::DELAY_BETWEEN_MEASUREMENTS_MILLIS);
        }
        printf("Measurements finished, made %ld measurements\n", Config::Common::MEASUREMENTS_NUM);
        uint64_t mean = time_measurements_sum / Config::Common::MEASUREMENTS_NUM;
        printf("Mean delay is %lld microseconds\n", mean);
        printf("Next measurement starts in %ld ms\n", Config::Common::DELAY_BETWEEN_MEASUREMENTS_BATCH_MILLIS);
        delay(Config::Common::DELAY_BETWEEN_MEASUREMENTS_BATCH_MILLIS);
    }
}

void solution_blocking()
{
    printf("Init...\n");
    Input input(Config::Input::INPUT_GPIO);
    input.init();

    Actuator actuator(Config::Actuator::ACTUATOR_GPIO, Actuator::ActuatorState::OFF);
    actuator.init();
    while (true)
    {
        printf("Starting measurement\n");
        uint64_t time_measurements_sum = 0;
        for (uint32_t i = 0; i < Config::Common::MEASUREMENTS_NUM; i++)
        {
            uint32_t result = measure_delay(&actuator, &input);
            delay(Config::Common::DELAY_BETWEEN_MEASUREMENTS_MILLIS);
            time_measurements_sum += result;
        }
        printf("Measurements finished, made %ld measurements\n", Config::Common::MEASUREMENTS_NUM);
        uint64_t mean = time_measurements_sum / Config::Common::MEASUREMENTS_NUM;
        printf("Mean delay is %lld microseconds\n", mean);
        printf("Next measurement starts in %ld ms\n", Config::Common::DELAY_BETWEEN_MEASUREMENTS_BATCH_MILLIS);
        delay(Config::Common::DELAY_BETWEEN_MEASUREMENTS_BATCH_MILLIS);
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
