#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED_GPIO GPIO_NUM_47   // change if your board uses a different pin

void app_main(void) {
    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    while (1) {
        gpio_set_level(LED_GPIO, 1); // LED ON
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        gpio_set_level(LED_GPIO, 0); // LED OFF
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}