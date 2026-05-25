#include "config.h"
#include "sma_filter.h"
#include "config.h"
#include <cassert>

SmaFilter::SmaFilter(const uint32_t switch_theshold, const uint32_t hysteresis_range) : buffer{}, switch_theshold(switch_theshold), hysteresis_range(hysteresis_range)
{
    is_initialized = false;
    current_index = 0;
    current_state_is_on = false;
}

void SmaFilter::process_new_sample(uint16_t sample)
{
    assert(sample < 1 << 12);
    if (!is_initialized)
    {
        for (auto &el : buffer)
        {
            el = sample;
        }
        is_initialized = true;
    }
    else
    {
        buffer[current_index] = sample;
    }

    current_index = (current_index + 1) % SMA_BUFFER_SIZE;
}

uint16_t SmaFilter::calculate_average()
{
    uint32_t sum = 0;
    for (auto el : buffer)
    {
        sum += el;
    }
    return (uint16_t)(sum / SMA_BUFFER_SIZE);
}

bool SmaFilter::is_on()
{
    uint16_t average = calculate_average();
    bool is_unknown = (average > (switch_theshold - hysteresis_range)) && (average < (switch_theshold + hysteresis_range));
    if (!is_unknown)
    {
        current_state_is_on = average < switch_theshold;
    }
    return current_state_is_on;
}
