#include "driver/gpio.h"
#include "config.h"
#include "ldr.h"

Ldr::Ldr(adc_unit_t unit_id, adc_channel_t channel) : _adc_handle(nullptr), _cali_handle(nullptr), _unit_id(unit_id), _channel(channel)
{
}

void Ldr::init()
{
    adc_oneshot_unit_init_cfg_t unit_config = {
        .unit_id = _unit_id,
    };
    adc_oneshot_new_unit(&unit_config, &_adc_handle);
    adc_oneshot_chan_cfg_t channel_config = {
        .atten = _ATTENUATION,
        .bitwidth = _RESOLUTION,
    };
    adc_oneshot_config_channel(_adc_handle, _channel, &channel_config);

    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = _unit_id,
        .chan = _channel,
        .atten = _ATTENUATION,
        .bitwidth = _RESOLUTION,
    };
    adc_cali_create_scheme_curve_fitting(&cali_config, &_cali_handle);
}

int Ldr::read_raw()
{
    int result = 0;
    adc_oneshot_read(_adc_handle, _channel, &result);
    return result;
}

int Ldr::read_calibrated()
{
    int raw = read_raw();
    int voltage_mv = 0;

    adc_cali_raw_to_voltage(_cali_handle, raw, &voltage_mv);
    return from_mv_to_digitized(voltage_mv);
}

int Ldr::from_mv_to_digitized(int milli_volts)
{
    if (milli_volts <= MIN_MV)
    {
        return 0;
    }
    if (milli_volts >= MAX_MV)
    {
        return RESOLUTION_MAX_VALUE;
    }
    return (milli_volts - MIN_MV) * RESOLUTION_MAX_VALUE / (MAX_MV - MIN_MV);
}

MaSmoothedLdr::MaSmoothedLdr(Ldr *ldr) : _ldr(ldr) {}

int MaSmoothedLdr::read_smoothed()
{
    int sensor_data = _ldr->read_calibrated();
    if (!_is_initialized)
    {
        for (int i = 0; i < _BUFFER_SIZE; i++)
        {
            _buffer[i] = sensor_data;
        }
        _next_element = 1;
        _is_initialized = true;
        return sensor_data;
    }

    _buffer[_next_element] = sensor_data;
    _next_element = (_next_element + 1) % _BUFFER_SIZE;
    return mean();
}

int MaSmoothedLdr::mean()
{
    int result = 0;
    for (int i = 0; i < _BUFFER_SIZE; i++)
    {
        result += _buffer[i];
    }
    return result / _BUFFER_SIZE;
}
