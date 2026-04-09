#pragma once

#include "driver/gpio.h"
#include "driver/ledc.h"

class Led
{
private:
    static const uint32_t FREQUENCY_HZ = 5000;
    static const ledc_mode_t SPEED_MODE = LEDC_LOW_SPEED_MODE;
    static const ledc_timer_bit_t RESOLUTION = LEDC_TIMER_12_BIT;
    static const ledc_timer_t TIMER = LEDC_TIMER_0;
    static const ledc_channel_t CHANNEL = LEDC_CHANNEL_0;

    gpio_num_t _gpio_num;

public:
    Led(gpio_num_t gpio_num);
    void init();
    void set_state(uint32_t state);
};