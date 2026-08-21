#include <driver/ledc.h>
#include "config.h"
#include "servo.h"
#include "ldr.h"

Servo::Servo(ledc_timer_t timer, ledc_channel_t channel, ledc_timer_bit_t resolution, uint32_t frequency, int pin) : _timer(timer), _channel(channel), _resolution(resolution), _frequency(frequency), _pin(pin)
{
}

void Servo::init()
{
    ledc_timer_config_t timer_config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = _resolution,
        .timer_num = _timer,
        .freq_hz = _frequency,
        .clk_cfg = LEDC_AUTO_CLK};

    ESP_ERROR_CHECK(ledc_timer_config(&timer_config));

    ledc_channel_config_t channel_config = {
        .gpio_num = _pin,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = _channel,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = _timer,
        .duty = 0};
    ESP_ERROR_CHECK(ledc_channel_config(&channel_config));
}

void Servo::set_angle(int digitized)
{
    int value_degrees = map_in_range(digitized, 0, LdrConfig::RESOLUTION_MAX_VALUE, ServoConfig::SERVO_MIN_DEG, ServoConfig::SERVO_MAX_DEG);
#ifdef DEBUG
    printf("value_degrees = %d\n", value_degrees);
#endif

    int value_micros = map_in_range(value_degrees, ServoConfig::SERVO_MIN_DEG, ServoConfig::SERVO_MAX_DEG, ServoConfig::SERVO_MIN_US, ServoConfig::SERVO_MAX_US);
#ifdef DEBUG
    printf("value_micros = %d\n", value_micros);
    printf("SERVO_MAX_DUTY = %ld\n", ServoConfig::SERVO_MAX_DUTY);
#endif

    uint32_t duty = (uint32_t)(ServoConfig::SERVO_MAX_DUTY / ServoConfig::SERVO_PERIOD_MS) * value_micros / 1000;
#ifdef DEBUG
    printf("Duty = %ld\n", duty);
#endif

    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, _channel, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, _channel));
}

int Servo::map_in_range(int value, int in_min, int in_max, int out_min, int out_max)
{
    return out_min + (value - in_min) * (out_max - out_min) / (in_max - in_min);
}
