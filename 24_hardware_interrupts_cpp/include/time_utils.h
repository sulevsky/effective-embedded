#pragma once

#include "stdint.h"

bool is_expired(uint32_t now, uint32_t last_event, uint32_t timeout);

uint32_t now_millis();

uint64_t now_micros();

void delay(uint32_t millis);
