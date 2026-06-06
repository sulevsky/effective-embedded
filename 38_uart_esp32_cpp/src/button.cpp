#include "button.h"
#include "driver/gpio.h"
#include "config.h"
#include "time_utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    Button *button = (Button *)arg;
    button->set_is_triggered_flag();
}

Button::Button(const gpio_num_t gpio_num) : _gpio_num(gpio_num)
{
    is_triggered_flag = false;
};

Button::~Button()
{
    gpio_reset_pin(_gpio_num);
    gpio_uninstall_isr_service();
}

void Button::init()
{
    gpio_reset_pin(_gpio_num);
    gpio_set_direction(_gpio_num, GPIO_MODE_INPUT);
    gpio_set_pull_mode(_gpio_num, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(_gpio_num, GPIO_INTR_NEGEDGE);

    gpio_install_isr_service(Config::Button::ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(_gpio_num, gpio_isr_handler, (void *)this);
}

IRAM_ATTR void Button::set_is_triggered_flag()
{
    is_triggered_flag = true;
}

void Button::reset_is_triggered_flag()
{
    is_triggered_flag = false;
}

bool Button::is_triggered()
{
    return is_triggered_flag;
}
