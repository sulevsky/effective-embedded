#pragma once

#include "esp_adc/adc_oneshot.h"

class Ldr
{
private:
    static const adc_atten_t _ATTENUATION = ADC_ATTEN_DB_12;
    // static const int MIN_MV = 0; // for ADC_ATTEN_DB_12, see ESP-IDF doc for conversion table
    // static const int MAX_MV = 3100; // for ADC_ATTEN_DB_12, see ESP-IDF doc for conversion table
    static const int MIN_MV = 20;   // measured
    static const int MAX_MV = 3173; // measured

    static const adc_bitwidth_t _RESOLUTION = ADC_BITWIDTH_12;
    adc_oneshot_unit_handle_t _adc_handle;
    adc_cali_handle_t _cali_handle;
    const adc_unit_t _unit_id;
    const adc_channel_t _channel;

public:
    static const uint32_t RESOLUTION_MAX_VALUE = ((1 << _RESOLUTION) - 1);
    Ldr(adc_unit_t unit_id, adc_channel_t channel);
    void init();
    int read_raw();
    int read_calibrated();
    int from_mv_to_digitized(int);
};

class MaSmoothedLdr
{
private:
    static const int _BUFFER_SIZE = 8;
    Ldr *_ldr;
    bool _is_initialized;
    int _buffer[_BUFFER_SIZE] = {0};
    int _next_element = 0;
    int mean();

public:
    MaSmoothedLdr(Ldr *ldr);
    int read_smoothed();
};
