#include "encoder.h"
#include "driver/pulse_cnt.h"

Encoder::Encoder(const gpio_num_t gpio_num_a, const gpio_num_t gpio_num_b) : gpio_num_a(gpio_num_a), gpio_num_b(gpio_num_b) {}

void Encoder::init()
{
    // unit
    pcnt_unit_config_t unit_config = {
        .low_limit = -1000,
        .high_limit = 1000,
    };
    pcnt_new_unit(&unit_config, &handle);

    // glitch filter
    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 1000,
    };
    pcnt_unit_set_glitch_filter(handle, &filter_config);

    // setup channel a
    pcnt_chan_config_t chan_a_config = {
        .edge_gpio_num = gpio_num_a,
        .level_gpio_num = gpio_num_b,
    };
    pcnt_channel_handle_t chan_a_handle = NULL;
    pcnt_new_channel(handle, &chan_a_config, &chan_a_handle);
    // setup channel b
    pcnt_chan_config_t chan_b_config = {
        .edge_gpio_num = gpio_num_b,
        .level_gpio_num = gpio_num_a,
    };
    pcnt_channel_handle_t chan_b_handle = NULL;
    pcnt_new_channel(handle, &chan_b_config, &chan_b_handle);

    // set action
    pcnt_channel_set_edge_action(chan_a_handle, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE);
    pcnt_channel_set_level_action(chan_a_handle, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE);
    pcnt_channel_set_edge_action(chan_b_handle, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE);
    pcnt_channel_set_level_action(chan_b_handle, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE);
    // enable
    pcnt_unit_enable(handle);
    pcnt_unit_clear_count(handle);
    pcnt_unit_start(handle);
}

int32_t Encoder::get_count()
{
    int result = 0;
    pcnt_unit_get_count(handle, &result);
    return static_cast<int32_t>(result / 4);
}
