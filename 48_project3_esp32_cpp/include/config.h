#pragma once

#include <driver/gpio.h>
#include <driver/ledc.h>
#include <esp_adc/adc_oneshot.h>

struct CommonConfig
{
    static const uint32_t SUPERLOOP_DELAY = 1000;
};

struct LdrConfig
{
    static const adc_unit_t ADC_UNIT = ADC_UNIT_1;
    static const adc_channel_t ADC_CHAN = ADC_CHANNEL_8;
    static const adc_atten_t ADC_ATTEN = ADC_ATTEN_DB_12;
    static const adc_bitwidth_t ADC_BITWIDTH = ADC_BITWIDTH_12;
    static const uint32_t RESOLUTION_MAX_VALUE = ((1 << ADC_BITWIDTH) - 1);
    // static const int MIN_MV = 0; // for ADC_ATTEN_DB_12, see ESP-IDF doc for conversion table
    // static const int MAX_MV = 3100; // for ADC_ATTEN_DB_12, see ESP-IDF doc for conversion table
    // original code
    // MIN_MV = 50;
    // MAX_MV = 3000;
    static const int MIN_MV = 20;   // measured
    static const int MAX_MV = 3173; // measured

    static constexpr uint32_t READ_PERIOD_MS = 20;
};

struct ServoConfig
{

    static const ledc_timer_t SERVO_TIMER = LEDC_TIMER_0;
    static const ledc_channel_t SERVO_CHANNEL = LEDC_CHANNEL_0;
    static const ledc_timer_bit_t SERVO_RESOLUTION = LEDC_TIMER_12_BIT;
    static const uint32_t SERVO_FREQ = 50;
    static const int SERVO_PIN = 14;

    // Calibration variables
    static const int SERVO_MIN_DEG = 10;
    static const int SERVO_MAX_DEG = 170;

    static const int SERVO_MIN_US = 500;
    static const int SERVO_MAX_US = 2500;

    static const uint32_t SERVO_PERIOD_MS = 1000 / SERVO_FREQ;
    static const uint32_t SERVO_MAX_DUTY = ((1 << SERVO_RESOLUTION) - 1);
};