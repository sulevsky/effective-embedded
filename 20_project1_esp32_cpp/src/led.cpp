#include "driver/gpio.h"
#include "config.h"
#include "led.h"
#include "driver/ledc.h"

Led::Led(gpio_num_t gpio_num)
{
    _gpio_num = gpio_num;
}

void Led::init()
{
    ledc_timer_config_t ledc_timer_configuration = {
        .speed_mode = SPEED_MODE,
        .duty_resolution = RESOLUTION,
        .timer_num = TIMER,
        .freq_hz = FREQUENCY_HZ,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&ledc_timer_configuration);

    ledc_channel_config_t channel_config = {
        .gpio_num = _gpio_num,
        .speed_mode = SPEED_MODE,
        .channel = CHANNEL,
        .timer_sel = TIMER,
        .duty = 0,
        .hpoint = 0,
    };
    ledc_channel_config(&channel_config);
}

void Led::set_state(uint32_t state)
{
    ledc_set_duty(SPEED_MODE, CHANNEL, state);
    ledc_update_duty(SPEED_MODE, CHANNEL);
}
