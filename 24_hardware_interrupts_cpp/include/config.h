#pragma once

#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"

struct CommonConfig
{
    static const uint32_t MILLIS_IN_SECONDS = 1000;
    static const uint32_t MICROS_IN_MILLIS = 1000;
    static const uint32_t LOG_PERIOD_MS = 1000;

    static const uint32_t LEVEL_HIGH = 1;
    static const uint32_t LEVEL_LOW = 0;
    static const uint32_t MEASUREMENTS_NUM = 10;
    static const uint32_t DELAY_BETWEEN_MEASUREMENTS_MILLIS = 1000;
    static const uint32_t DELAY_BETWEEN_EXPERIMENTS_BATCH_MILLIS = 5000;
};

struct ButtonConfig
{
    static const gpio_num_t BUTTON_GPIO = GPIO_NUM_4;
    static const int ESP_INTR_FLAG_DEFAULT = 0;

    static const uint32_t DEFAULT_DEBOUNCE_DELAY_MICROS = 50000;
    
    static const uint32_t HELD_LONG_MIN_TIME_MICROS = 2000000;
    
    static const uint32_t POLL_DELAY_MILLIS = 5;
};

struct ExperimentConfig
{
    static const uint32_t NUMBER_OF_PRESSES_PER_EXPERIMENT = 5;
    static const uint32_t EXPERIMENT_TIME_MILLIS = 10000;
    
    
};
