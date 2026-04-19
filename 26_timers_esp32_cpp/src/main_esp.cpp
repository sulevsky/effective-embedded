#ifndef DRIVER_API

#include "config.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "switch.h"
#include "non_volatile_storage.h"

static volatile uint32_t MINUTES_COUNTER = 0;
static volatile bool IS_ON = true;

bool define_is_on(uint32_t counter)
{
    return counter < SwitchConfig::SWITCH_IS_ON_MINUTES;
}

static void timer_isr_handler(void *arg)
{
    MINUTES_COUNTER = (MINUTES_COUNTER + 1) % CommonConfig::MINUTE_IN_HOUR;
    bool new_is_on = define_is_on(MINUTES_COUNTER);
    if (new_is_on != IS_ON)
    {
        // allowed here
        esp_rom_printf("in interrupt, switching is on %d, minute %d\n", new_is_on, MINUTES_COUNTER);
        IS_ON = new_is_on;
        set_switch(SwitchConfig::SWITCH_GPIO, IS_ON);
    }
    // allowed here
    esp_rom_printf("in interrupt, minute %d\n", MINUTES_COUNTER);
    save(MINUTES_COUNTER);
}

extern "C" void app_main(void)
{
    printf("Started...\n");
    init_switch(SwitchConfig::SWITCH_GPIO);

    init_non_volatile_storage();
    uint32_t saved_counter = read();
    esp_rom_printf("Restored counter %d\n", saved_counter);
    MINUTES_COUNTER = saved_counter;
    IS_ON = define_is_on(MINUTES_COUNTER);

    esp_timer_handle_t timer_handle = NULL;
    esp_timer_create_args_t timer_config = {
        .callback = &timer_isr_handler,
    };

    esp_timer_create(&timer_config, &timer_handle);
    esp_timer_start_periodic(timer_handle, CommonConfig::MINUTE_PERIOD);

    while (true)
    {
    }
}

#endif
