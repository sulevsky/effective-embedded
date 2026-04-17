#include "button.h"
#include "driver/gpio.h"
#include "config.h"
#include "time_utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

volatile uint32_t counter = 0;
volatile bool is_triggered_flag = false;

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    counter += 1;
    is_triggered_flag = true;
}

Button::Button(gpio_num_t gpio_num) : _gpio_num(gpio_num) {};

Button::~Button()
{
    reset();
    gpio_reset_pin(_gpio_num);
    gpio_uninstall_isr_service();
}

void Button::init(gpio_int_type_t intr_type)
{
    gpio_reset_pin(_gpio_num);
    gpio_set_direction(_gpio_num, GPIO_MODE_INPUT);
    gpio_set_pull_mode(_gpio_num, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(_gpio_num, intr_type);

    gpio_install_isr_service(ButtonConfig::ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(ButtonConfig::BUTTON_GPIO, gpio_isr_handler, (void *)ButtonConfig::BUTTON_GPIO);
}

uint32_t Button::get_counter()
{
    return counter;
}

void Button::reset()
{
    counter = 0;
    is_triggered_flag = false;
}

bool Button::is_triggered()
{
    return is_triggered_flag;
}

void Button::reset_triggered_flag()
{
    is_triggered_flag = false;
}

bool Button::is_pressed()
{
    return gpio_get_level(_gpio_num) == 0;
}
