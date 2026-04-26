#pragma once

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"

class Input
{
private:
    const gpio_num_t _gpio_num;

public:
    QueueHandle_t queue;
    Input(const gpio_num_t gpio_num);
    void init();
    bool is_on();
    uint64_t get_measurement_end_blocking();
};
