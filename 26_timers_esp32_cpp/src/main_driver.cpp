#ifdef DRIVER_API

#include "driver/gpio.h"
#include "config.h"
#include "time_utils.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_cpu.h"
#include "driver/gptimer.h"
#include "esp_log.h"
#include "switch.h"

static volatile uint32_t MINUTES_COUNTER = 0;
static bool IRAM_ATTR timer_isr_handler(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    // not allowed here
    // esp_rom_printf("in interrupt\n");
    MINUTES_COUNTER = (MINUTES_COUNTER + 1) % CommonConfig::MINUTE_IN_HOUR;
    return true;
}

extern "C" void app_main(void)
{
    printf("Started...\n");

    init_switch(SwitchConfig::SWITCH_GPIO);

    gptimer_handle_t timer_handle = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = CommonConfig::MINUTE_PERIOD,

    };
    gptimer_new_timer(&timer_config, &timer_handle);

    gptimer_event_callbacks_t calback = {.on_alarm = timer_isr_handler};
    gptimer_register_event_callbacks(timer_handle, &calback, NULL);
    gptimer_enable(timer_handle);

    gptimer_alarm_config_t alarm_config = {
        .alarm_count = CommonConfig::MINUTE_PERIOD,
        .flags = {.auto_reload_on_alarm = true},
    };
    gptimer_set_alarm_action(timer_handle, &alarm_config);
    gptimer_start(timer_handle);

    bool current_is_on = true;
    while (true)
    {
        bool new_is_on = MINUTES_COUNTER < SwitchConfig::SWITCH_IS_ON_MINUTES;
        if (current_is_on != new_is_on)
        {
            esp_rom_printf("Minutes: %d, is on %d\n", MINUTES_COUNTER, new_is_on);
            set_switch(SwitchConfig::SWITCH_GPIO, new_is_on);
            current_is_on = new_is_on;
        }
    }
}

#endif
