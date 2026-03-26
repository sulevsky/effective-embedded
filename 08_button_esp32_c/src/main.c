#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BLUE_LED_GPIO GPIO_NUM_4
#define RED_LED_GPIO GPIO_NUM_5
#define LEVEL_HIGH 1
#define LEVEL_LOW 0

#define EXTERNAL_BUTTON_GPIO GPIO_NUM_19
#define BUTTON_BOOT GPIO_NUM_0

#define FAST_PERIOD 100
#define SLOW_PERIOD 4000

#define DEBOUNCING_TIME 200
#define LOOP_STEP 50

void delay_ms(uint32_t millis)
{
    vTaskDelay(millis / portTICK_PERIOD_MS);
}

bool is_first(uint32_t counter, uint32_t full_period, uint32_t first_period)
{
    return first_period >= (counter % full_period);
}

void app_main(void)
{
    printf("Init...\n");
    gpio_reset_pin(BLUE_LED_GPIO);
    gpio_reset_pin(RED_LED_GPIO);
    gpio_reset_pin(EXTERNAL_BUTTON_GPIO);
    gpio_reset_pin(BUTTON_BOOT);

    gpio_set_direction(BLUE_LED_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(RED_LED_GPIO, GPIO_MODE_OUTPUT);

    gpio_set_direction(EXTERNAL_BUTTON_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(EXTERNAL_BUTTON_GPIO, GPIO_PULLUP_ONLY);

    gpio_set_direction(BUTTON_BOOT, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_BOOT, GPIO_PULLUP_ONLY);

    uint32_t period = SLOW_PERIOD;

    uint32_t counter = 1;

    uint32_t button_a_unavailable_until = 0;
    uint32_t button_boot_unavailable_until = 0;
    bool current_is_first = true;
    while (true)
    {

        if (button_a_unavailable_until < counter)
        {
            if (gpio_get_level(EXTERNAL_BUTTON_GPIO) == 0)
            {
                period = FAST_PERIOD;
                button_a_unavailable_until = counter + DEBOUNCING_TIME;
                printf("External button is pressed\n");
            }
        }
        if (button_boot_unavailable_until < counter)
        {
            if (gpio_get_level(BUTTON_BOOT) == 0)
            {
                period = SLOW_PERIOD;
                button_boot_unavailable_until = counter + DEBOUNCING_TIME;
                printf("GPIO button is pressed\n");
            }
        }
        bool next_is_first = is_first(counter, period * 2, period);
        if (current_is_first != next_is_first)
        {
            if (next_is_first)
            {
                gpio_set_level(BLUE_LED_GPIO, LEVEL_HIGH);
                gpio_set_level(RED_LED_GPIO, LEVEL_LOW);
            }
            else
            {
                gpio_set_level(BLUE_LED_GPIO, LEVEL_LOW);
                gpio_set_level(RED_LED_GPIO, LEVEL_HIGH);
            }
            current_is_first = next_is_first;
        }
        delay_ms(LOOP_STEP);
        counter += LOOP_STEP;
    }
}
