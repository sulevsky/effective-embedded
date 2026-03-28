// example https://github.com/espressif/esp-idf/blob/master/examples/peripherals/gpio/generic_gpio/main/gpio_example_main.c

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BLUE_LED_GPIO GPIO_NUM_4
#define LEVEL_HIGH 1
#define LEVEL_LOW 0

#define EXTERNAL_BUTTON_GPIO GPIO_NUM_19
#define GPIO_OUTPUT_PIN_SEL (1ULL << EXTERNAL_BUTTON_GPIO)
#define ESP_INTR_FLAG_DEFAULT 0

static int16_t counter = 0;

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    counter++;
    esp_rom_printf("Button is  Pressed! GPIO: %ld Count: %d\n", gpio_num, counter);
}

void app_main(void)
{
    printf("Init...\n");
    // zero-initialize the config structure.
    gpio_config_t io_conf = {
        // interrupt of falling edge
        .intr_type = GPIO_INTR_NEGEDGE,
        // bit mask of the pins
        .pin_bit_mask = GPIO_OUTPUT_PIN_SEL,
        // set as input mode
        .mode = GPIO_MODE_INPUT,
        // enable pull-up mode
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    gpio_config(&io_conf);

    // install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    // hook isr handler for specific gpio pin
    gpio_isr_handler_add(EXTERNAL_BUTTON_GPIO, gpio_isr_handler, (void *)EXTERNAL_BUTTON_GPIO);

    gpio_reset_pin(BLUE_LED_GPIO);
    gpio_set_direction(BLUE_LED_GPIO, GPIO_MODE_OUTPUT);

    while (true)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
