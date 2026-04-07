#include "time_utils.h"
#include "stdint.h"

bool is_expired(uint32_t now, uint32_t last_event, uint32_t timeout)
{
    return (now - last_event) >= timeout;
}

bool is_first(uint32_t counter, uint32_t full_period, uint32_t first_period)
{
    return first_period >= (counter % full_period);
}
