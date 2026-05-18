#include "driver/gpio.h"
#include "esp_timer.h"
#include "config.h"
#include "time_utils.h"
#include "esp_cpu.h"
#include "switch.h"
#include "led.h"
#include "driver/gptimer.h"

typedef enum
{
    GREEN,
    GREEN_BLINKING,
    YELLOW,
    YELLOW_BLINKING,
    RED,
    RED_YELLOW,
} state_t;

Switch switcher(SwitchConfig::SWITCH_GPIO);

Led red(LedConfig::RED_GPIO, LedConfig::RED_BLINKING_PERIOD_MILLIS);
Led yellow(LedConfig::YELLOW_GPIO, LedConfig::YELLOW_BLINKING_PERIOD_MILLIS);
Led green(LedConfig::GREEN_GPIO, LedConfig::GREEN_BLINKING_PERIOD_MILLIS);

volatile state_t next_state = RED;

static bool IRAM_ATTR timer_isr_handler_state_transition(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    int delay = 0;
    switch (next_state)
    {
    case GREEN:
        red.off();
        yellow.off();
        green.on();

        next_state = GREEN_BLINKING;
        delay = LedConfig::GREEN_DURATION_SECONDS;
        break;
    case GREEN_BLINKING:
        red.off();
        yellow.off();
        green.blinking();

        next_state = YELLOW;
        delay = LedConfig::GREEN_BLINKING_DURATION_SECONDS;
        break;
    case YELLOW:
        red.off();
        yellow.on();
        green.off();

        next_state = RED;
        delay = LedConfig::YELLOW_DURATION_SECONDS;
        break;
    case YELLOW_BLINKING:
        red.off();
        yellow.blinking();
        green.off();

        if (switcher.is_on())
        {
            delay = LedConfig::YELLOW_BLINKING_DURATION_SECONDS;
            next_state = YELLOW_BLINKING;
        }
        else
        {
            delay = LedConfig::YELLOW_BLINKING_DURATION_SECONDS;
            next_state = RED;
        }
        break;
    case RED:
        red.on();
        yellow.off();
        green.off();

        if (switcher.is_on())
        {
            delay = LedConfig::RED_DURATION_SECONDS;
            next_state = YELLOW_BLINKING;
        }
        else
        {
            delay = LedConfig::RED_DURATION_SECONDS;
            next_state = RED_YELLOW;
        }
        break;
    case RED_YELLOW:
        red.on();
        yellow.on();
        green.off();

        delay = LedConfig::RED_YELLOW_DURATION_SECONDS;
        next_state = GREEN;
        break;

    default:
        break;
    }
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = delay * CommonConfig::MICROS_IN_MILLIS * CommonConfig::MILLIS_IN_SECONDS,
        .flags = {.auto_reload_on_alarm = true},
    };
    gptimer_set_alarm_action(timer, &alarm_config);
    return true;
}

volatile uint32_t millis_counter = 0;

static bool IRAM_ATTR timer_isr_handler_blink(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    uint32_t count = edata->alarm_value / CommonConfig::MICROS_IN_MILLIS;
    millis_counter += count;
    red.on_timer_event(millis_counter);
    yellow.on_timer_event(millis_counter);
    green.on_timer_event(millis_counter);

    return true;
}

extern "C" void app_main(void)
{
    printf("Init..");

    switcher.init();

    red.init();
    yellow.init();
    green.init();

    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = CommonConfig::MICROSECOND_RESOLUTION,

    };

    // 1 switch
    gptimer_handle_t timer_handle_state = NULL;
    gptimer_new_timer(&timer_config, &timer_handle_state);

    gptimer_event_callbacks_t calback = {.on_alarm = timer_isr_handler_state_transition};
    gptimer_register_event_callbacks(timer_handle_state, &calback, NULL);
    gptimer_enable(timer_handle_state);

    gptimer_alarm_config_t alarm_config = {
        .alarm_count = CommonConfig::INITIAL_ALARM_COUNT,
        .flags = {.auto_reload_on_alarm = true},
    };
    gptimer_set_alarm_action(timer_handle_state, &alarm_config);
    gptimer_start(timer_handle_state);

    // 2 blink
    gptimer_handle_t timer_handle_blink = NULL;
    gptimer_new_timer(&timer_config, &timer_handle_blink);

    gptimer_event_callbacks_t calback_blink = {.on_alarm = timer_isr_handler_blink};
    gptimer_register_event_callbacks(timer_handle_blink, &calback_blink, NULL);
    gptimer_enable(timer_handle_blink);

    gptimer_alarm_config_t alarm_config_blink = {
        .alarm_count = LedConfig::BLINKING_CHECK_INTERVAL_MICROS,
        .flags = {.auto_reload_on_alarm = true},
    };
    gptimer_set_alarm_action(timer_handle_blink, &alarm_config_blink);
    gptimer_start(timer_handle_blink);

    while (true)
    {
        esp_cpu_wait_for_intr();
    }
}
