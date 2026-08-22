#pragma once

#include "stdint.h"
#include "stdbool.h"

inline bool is_expired(uint32_t now, uint32_t last_event, uint32_t timeout)
{
    return (now - last_event) >= timeout;
}
