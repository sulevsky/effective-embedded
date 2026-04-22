#include "driver/gpio.h"
#include "esp_timer.h"
#include "led.h"
#include "button.h"
#include "config.h"
#include "time_utils.h"

using namespace Config;

extern "C" void app_main(void)
{

    Led red(GPIO::RED_LED_GPIO, LedState::ON);
    red.init();
    Led blue(GPIO::BLUE_LED_GPIO, LedState::OFF);
    blue.init();

    Button extenal_button(GPIO::EXTERNAL_BUTTON_GPIO);
    extenal_button.init();
    Button boot_button(GPIO::BUTTON_BOOT);
    boot_button.init();

    uint32_t period = Blink::SLOW_PERIOD;

    uint32_t last_external_button_press = 0;
    uint32_t last_boot_button_press = 0;

    bool current_is_first = true;
    uint32_t iterations_num = 0;
    while (true)
    {
        uint32_t now = (uint32_t)(esp_timer_get_time() / Common::MILLIS_IN_MICROS);
        if (is_expired(now, last_external_button_press, Blink::DEBOUNCING_TIME))
        {
            if (extenal_button.is_pressed())
            {
                period = Blink::FAST_PERIOD;
                last_external_button_press = now;
                printf("External button is pressed\n");
            }
        }
        if (is_expired(now, last_boot_button_press, Blink::DEBOUNCING_TIME))
        {
            if (boot_button.is_pressed())
            {
                period = Blink::SLOW_PERIOD;
                last_boot_button_press = now;
                printf("Boot button is pressed\n");
            }
        }
        bool next_is_first = is_first(now, period * 2, period);
        if (current_is_first != next_is_first)
        {
            if (next_is_first)
            {
                blue.set_state(LedState::ON);
                red.set_state(LedState::OFF);
            }
            else
            {
                blue.set_state(LedState::OFF);
                red.set_state(LedState::ON);
            }
            current_is_first = next_is_first;
        }
        iterations_num++;
        if (iterations_num % Common::DURATION_LOG_ONCE_PER == 0)
        {
            printf("Time: %lld, mean iteration time: %lld micros, number of iterations %ld\n", esp_timer_get_time(), (esp_timer_get_time() / iterations_num), iterations_num);
        }
    }
}
