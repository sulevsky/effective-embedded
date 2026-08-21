#pragma once

#include <driver/ledc.h>

class Servo
{
private:
    const ledc_timer_t _timer;
    const ledc_channel_t _channel;
    const ledc_timer_bit_t _resolution;
    const uint32_t _frequency;
    const int _pin;
    int map_in_range(int value, int in_min, int in_max, int out_min, int out_max);

public:
    Servo(ledc_timer_t timer, ledc_channel_t channel, ledc_timer_bit_t resolution, uint32_t frequency, int pin);
    void init();
    /**
     * 0 - 4095 range
     */
    void set_angle(int digitized);
};
