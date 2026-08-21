#pragma once

#include "esp_adc/adc_oneshot.h"

class Ldr
{
private:
    adc_oneshot_unit_handle_t _adc_handle;
    adc_cali_handle_t _cali_handle;
    const adc_unit_t _unit_id;
    const adc_channel_t _channel;
    const adc_bitwidth_t _resolution;
    const adc_atten_t _attenuation;

public:
    Ldr(adc_unit_t unit_id, adc_channel_t channel, adc_bitwidth_t resolution, adc_atten_t attenuation);
    void init();
    int read_raw();
    int convert_to_calibrated_mv(int raw);
    int read_calibrated_and_scaled();
    int scale_from_mv_to_digitized(int);
};
