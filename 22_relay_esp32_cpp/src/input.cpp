#include "input.h"
#include "driver/gpio.h"
#include "config.h"
#include "time_utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

volatile uint64_t measurement_end = 0;
volatile gpio_num_t GPIO_NUM = GPIO_NUM_NC;

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    // measurement_end == 0 is debouncing
    if (gpio_num == GPIO_NUM && measurement_end == 0)
    {
        measurement_end = now_micros();
    }
}

Input::Input(gpio_num_t gpio_num) : _gpio_num(gpio_num)
{
    GPIO_NUM = gpio_num;
}

void Input::init()
{
    gpio_reset_pin(_gpio_num);
    gpio_set_direction(_gpio_num, GPIO_MODE_INPUT);
    gpio_set_pull_mode(_gpio_num, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(_gpio_num, GPIO_INTR_NEGEDGE);

    gpio_install_isr_service(InputConfig::ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(InputConfig::INPUT_GPIO, gpio_isr_handler, (void *)InputConfig::INPUT_GPIO);
}

bool Input::is_on()
{
    return gpio_get_level(_gpio_num) == 0;
}

uint64_t Input::get_measurement_end()
{
    return measurement_end;
}

void Input::reset_measurement()
{
    measurement_end = 0;
}
