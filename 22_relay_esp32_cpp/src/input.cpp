#include "input.h"
#include "driver/gpio.h"
#include "config.h"
#include "time_utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    Input *input = (Input *)arg;
    uint64_t now = now_micros();
    BaseType_t woken = pdFALSE;
    xQueueSendFromISR(input->queue, &now, &woken);
    portYIELD_FROM_ISR(woken);
}

Input::Input(gpio_num_t gpio_num) : _gpio_num(gpio_num), queue(NULL)
{
}

void Input::init()
{
    queue = xQueueCreate(1, sizeof(uint64_t));
    gpio_reset_pin(_gpio_num);
    gpio_set_direction(_gpio_num, GPIO_MODE_INPUT);
    gpio_set_pull_mode(_gpio_num, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(_gpio_num, GPIO_INTR_NEGEDGE);

    gpio_install_isr_service(Config::Input::ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(_gpio_num, gpio_isr_handler, (void *)this);
}

bool Input::is_on()
{
    return gpio_get_level(_gpio_num) == 0;
}

uint64_t Input::get_measurement_end_blocking()
{
    uint64_t measurement_end = 0;
    xQueueReceive(this->queue, &measurement_end, Config::Input::MEASUREMENT_WAIT_TIME);
    return measurement_end;
}
