#pragma once

#include "stdint.h"

inline bool is_expired(const uint32_t now, const uint32_t last_event, const uint32_t timeout)
{
    return (now - last_event) >= timeout;
}

inline bool is_first(const uint32_t counter, const uint32_t full_period, const uint32_t first_period)
{
    return first_period >= (counter % full_period);
}
