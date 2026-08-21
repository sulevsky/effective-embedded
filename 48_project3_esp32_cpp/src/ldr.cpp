#include "driver/gpio.h"
#include "config.h"
#include "ldr.h"

Ldr::Ldr(adc_unit_t unit_id, adc_channel_t channel, adc_bitwidth_t resolution, adc_atten_t attenuation) : _adc_handle(nullptr), _cali_handle(nullptr), _unit_id(unit_id), _channel(channel), _resolution(resolution), _attenuation(attenuation)
{
}

void Ldr::init()
{
    adc_oneshot_unit_init_cfg_t unit_config = {
        .unit_id = _unit_id,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_config, &_adc_handle));
    adc_oneshot_chan_cfg_t channel_config = {
        .atten = _attenuation,
        .bitwidth = _resolution,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(_adc_handle, _channel, &channel_config));

    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = _unit_id,
        .chan = _channel,
        .atten = _attenuation,
        .bitwidth = _resolution,
    };
    ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&cali_config, &_cali_handle));
}

int Ldr::read_raw()
{
    int result = 0;
    ESP_ERROR_CHECK(adc_oneshot_read(_adc_handle, _channel, &result));
    return result;
}

int Ldr::convert_to_calibrated_mv(int raw)
{
    int voltage_mv = 0;

    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(_cali_handle, raw, &voltage_mv));
    return voltage_mv;
}

int Ldr::read_calibrated_and_scaled()
{
    int raw = read_raw();
    int voltage_mv = convert_to_calibrated_mv(raw);
    return scale_from_mv_to_digitized(voltage_mv);
}

int Ldr::scale_from_mv_to_digitized(int milli_volts)
{
    if (milli_volts <= LdrConfig::MIN_MV)
    {
        return 0;
    }
    if (milli_volts >= LdrConfig::MAX_MV)
    {
        return LdrConfig::RESOLUTION_MAX_VALUE;
    }
    return (milli_volts - LdrConfig::MIN_MV) * LdrConfig::RESOLUTION_MAX_VALUE / (LdrConfig::MAX_MV - LdrConfig::MIN_MV);
}
