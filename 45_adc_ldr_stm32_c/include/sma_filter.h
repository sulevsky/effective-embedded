#pragma once

#include "stdint.h"
#include "config.h"

class SmaFilter
{
private:
    const uint32_t switch_theshold;
    const uint32_t hysteresys_range;
    uint16_t buffer[SMA_BUFFER_SIZE];
    bool is_initialized;
    uint32_t current_index;
    bool current_state_is_on;
    uint16_t calculate_average();

public:
    SmaFilter(const uint32_t switch_theshold, const uint32_t hysteresys_range);
    void process_new_sample(uint16_t sample);
    bool is_on();
};
