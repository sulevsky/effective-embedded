#include "driver/gpio.h"

#define BLUE_LED_GPIO GPIO_NUM_4
#define RED_LED_GPIO GPIO_NUM_5
#define LEVEL_HIGH 1
#define LEVEL_LOW 0

void delay();

void app_main(void)
{
    gpio_reset_pin(BLUE_LED_GPIO);
    gpio_reset_pin(RED_LED_GPIO);
    gpio_set_direction(BLUE_LED_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(RED_LED_GPIO, GPIO_MODE_OUTPUT);

    while (1)
    {
        gpio_set_level(BLUE_LED_GPIO, LEVEL_HIGH);
        gpio_set_level(RED_LED_GPIO, LEVEL_LOW);
        delay();

        gpio_set_level(BLUE_LED_GPIO, LEVEL_LOW);
        gpio_set_level(RED_LED_GPIO, LEVEL_HIGH);
        delay();
    }
}

void delay()
{
    for (int i = 0; i < 5000000; i++)
    {
        asm("nop");
    }
}