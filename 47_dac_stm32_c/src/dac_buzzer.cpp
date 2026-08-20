#include "config.h"
#include "dac_buzzer.h"
#include <stdio.h>
#include "stdint.h"
#include "time_utils.h"
#include "time_hal.h"
#include "stm32f4xx_hal.h"

DacBuzzer::DacBuzzer(DAC_HandleTypeDef *hdac, const uint32_t channel, const uint32_t alignment) : hdac(hdac), channel(channel), alignment(alignment)
{
}

void DacBuzzer::start()
{
    HAL_DAC_Start(hdac, channel);
}

void DacBuzzer::set_value(uint16_t value)
{
    HAL_DAC_SetValue(hdac, channel, alignment, value);
}
const uint16_t SIN_DAC_VALUES[] = {
    2048,
    2855,
    3532,
    3968,
    4093,
    3886,
    3382,
    2661,
    1840,
    1054,
    428,
    65,
    23,
    309,
    878,
    1635,
};

void DacBuzzer::play(uint16_t frequency, uint32_t duration_millis)
{
    uint32_t play_start = now_millis();
    if (frequency == 0)
    {
        set_value(0);
        while (!is_expired(now_millis(), play_start, duration_millis))
        {
            // busy wait for full period
        }
        return;
    }
    uint32_t period_duration_micros = MICROS_IN_SECOND / frequency;
    uint32_t step_duration_micros = period_duration_micros / SIN_FUNCTION_RESOLUTION;
    uint32_t num_periods = 0;
    while (!is_expired(now_millis(), play_start, duration_millis))
    {
        uint32_t period_start_micros = now_micros();
        uint32_t cummulative_step_duration_micros = step_duration_micros;
        for (int step = 0; step < SIN_FUNCTION_RESOLUTION; step++)
        {
            u_int16_t dac_value = SIN_DAC_VALUES[step];
            while (!is_expired(now_micros(), period_start_micros, cummulative_step_duration_micros))
            {
            }
            cummulative_step_duration_micros += step_duration_micros;
        }
    }
    set_value(0);
    printf("num_periods %d\r\n", num_periods);
}
