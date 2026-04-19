#pragma once

#include "stdint.h"

void init_non_volatile_storage();
void save(uint32_t value);
uint32_t read();
