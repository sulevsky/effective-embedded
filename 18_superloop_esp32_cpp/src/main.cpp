#include "driver/gpio.h"
#include "esp_timer.h"
#include "led.h"
#include "config.h"
#include "time_utils.h"

extern "C" void app_main(void)
{

    Led red(GPIOConfig::RED_LED_GPIO, OFF);
    red.init();
    Led blue(GPIOConfig::BLUE_LED_GPIO, OFF);
    blue.init();
    Led green(GPIOConfig::GREEN_LED_GPIO, OFF);
    green.init();

    uint32_t last_red_toggle = 0;
    uint32_t last_blue_toggle = 0;
    uint32_t last_green_toggle = 0;

    bool current_is_first = true;
    while (true)
    {
        uint32_t now = (uint32_t)(esp_timer_get_time() / CommonConfig::MILLIS_IN_MICROS);
        if (is_expired(now, last_red_toggle, BlinkConfig::RED_PERIOD_MS))
        {
            red.toggle();
            last_red_toggle = now;
        }
        if (is_expired(now, last_blue_toggle, BlinkConfig::BLUE_PERIOD_MS))
        {
            blue.toggle();
            last_blue_toggle = now;
        }
        if (is_expired(now, last_green_toggle, BlinkConfig::GREEN_PERIOD_MS))
        {
            green.toggle();
            last_green_toggle = now;
        }
    }
}
