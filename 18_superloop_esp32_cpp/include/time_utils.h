#pragma once

#include "stdint.h"

bool is_expired(const uint32_t now, const uint32_t last_event, const uint32_t timeout);
