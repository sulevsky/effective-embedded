#pragma once

#include "driver/gpio.h"

void set_switch(gpio_num_t gpio, bool is_on);
void init_switch(gpio_num_t gpio);
