#include "time_utils.h"
#include "stdint.h"

bool is_expired(uint32_t now, uint32_t last_event, uint32_t timeout)
{
    return (now - last_event) >= timeout;
}
