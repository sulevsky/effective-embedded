#include "config.h"
#include "gpio.h"
#include "square_buzzer.h"
#include "time_utils.h"
#include "time_hal.h"
#include "stdio.h"

SquareBuzzer::SquareBuzzer(const Gpio gpio) : gpio(gpio)
{
}

void SquareBuzzer::set_state(bool is_on)
{
    if (is_on)
    {
        HAL_GPIO_WritePin(gpio.port, gpio.pin, GPIO_PinState::GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(gpio.port, gpio.pin, GPIO_PinState::GPIO_PIN_RESET);
    }
}

void SquareBuzzer::play(uint16_t frequency, uint32_t duration_millis)
{
    uint32_t play_start = now_millis();
    if (frequency == 0)
    {
        set_state(false);
        while (!is_expired(now_millis(), play_start, duration_millis))
        {
            // busy wait for full period
        }
        return;
    }
    uint32_t period_duration_micros = MICROS_IN_SECOND / frequency;
    uint32_t half_period_duration_micros = period_duration_micros / 2;
    while (!is_expired(now_millis(), play_start, duration_millis))
    {
        set_state(false);
        uint32_t period_start_micros = now_micros();
        while (!is_expired(now_micros(), period_start_micros, half_period_duration_micros))
        {
        }
        set_state(true);
        while (!is_expired(now_micros(), period_start_micros, period_duration_micros))
        {
        }
    }
    set_state(false);
}
