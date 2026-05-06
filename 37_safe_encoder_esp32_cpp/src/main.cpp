#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "esp_timer.h"
#include "config.h"
#include "time_utils.h"
#include "esp_cpu.h"
#include "encoder.h"
#include "button.h"
#include "led.h"
#include "safe_state.h"
#include "renderer.h"

void reset(SafeState *safe_state)
{
    safe_state->reset();
    safe_state->increment_tries_taken();
}

extern "C" void app_main(void)
{
    // uart init delay
    vTaskDelay(pdMS_TO_TICKS(3000));
    printf("Init..\n");

    Led red(Config::Led::RED_GPIO);
    red.init();
    Led green(Config::Led::GREEN_GPIO);
    green.init();

    Encoder encoder(Config::Encoder::ENCODER_GPIO_A, Config::Encoder::ENCODER_GPIO_B);
    encoder.init();

    Button button(Config::Button::BUTTON_GPIO);
    button.init();

    SafeState safe_state;
    // todo vova assumes first is CW
    // todo vova test overflow
    // todo vova handle overflow and minus
    // todo vova debounce button
    
    // start with inputing correct code
    // while (true)
    {
        draw(&safe_state);
        if (safe_state.are_tries_consumed())
        {
            vTaskDelay(pdMS_TO_TICKS(5000));
            safe_state.reset();
            safe_state.reset_tries();
            continue;
        }
        if (button.is_triggered())
        {
            reset(&safe_state);
            button.reset_is_triggered_flag();
            continue;
        }
        uint8_t new_counter = encoder.get_count();
        if (!safe_state.is_counter_updated(new_counter))
        {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }
        if (safe_state.is_direction_same(new_counter))
        {
            safe_state.increment_digit();
            safe_state.update_counter(new_counter);
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }
        if (!safe_state.is_last_digit())
        {
            safe_state.change_direction();
            safe_state.increment_position();
            safe_state.update_counter(new_counter);
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }
        if (safe_state.is_matching_code())
        {
            vTaskDelay(pdMS_TO_TICKS(5000));
        }
        else
        {
            reset(&safe_state);
            button.reset_is_triggered_flag();
            safe_state.update_counter(new_counter);
        }
    }
}
