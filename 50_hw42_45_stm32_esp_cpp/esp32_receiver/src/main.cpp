#include "esp_timer.h"
#include "led.h"
#include "config.h"

extern "C" void app_main(void)
{
    Led leds[] = {
        Led(Config::GPIO::RED_LED_GPIO, Led::State::OFF, Config::Blink::RED_PERIOD_MS),
        Led(Config::GPIO::BLUE_LED_GPIO, Led::State::OFF, Config::Blink::BLUE_PERIOD_MS),
        Led(Config::GPIO::GREEN_LED_GPIO, Led::State::OFF, Config::Blink::GREEN_PERIOD_MS),
    };
    for (auto &led : leds)
    {
        led.init();
    }

    while (true)
    {
        uint32_t now = (uint32_t)(esp_timer_get_time() / Config::Common::MILLIS_IN_MICROS);
        for (auto &led : leds)
        {
            led.toggle_if_expired(now);
        }
    }
}
