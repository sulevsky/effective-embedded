#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "esp_timer.h"
#include "config.h"
#include "time_utils.h"
#include "esp_cpu.h"
#include "encoder.h"
#include "button.h"
#include "safe_state.h"
#include "renderer.h"

void reset(SafeState *safe_state)
{
    safe_state->reset();
    safe_state->increment_tries_taken();
}

extern "C" void app_main(void)
{
    delay(Config::Common::UART_INIT_DELAY_MILLIS);
    printf("Init..\n");

    Encoder encoder(Config::Encoder::ENCODER_GPIO_A, Config::Encoder::ENCODER_GPIO_B);
    encoder.init();

    Button button(Config::Button::BUTTON_GPIO);
    button.init();

    SafeState safe_state;

    while (true)
    {
        draw(&safe_state);
        if (safe_state.are_tries_consumed())
        {
            delay(Config::Common::BLOCKED_DELAY_MILLIS);
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
        int32_t new_counter = encoder.get_count();
        if (!safe_state.is_counter_updated(new_counter))
        {
            delay(Config::Common::ITERATION_DELAY_MILLIS);
            continue;
        }
        if (safe_state.is_direction_same(new_counter))
        {
            safe_state.increment_digit();
            safe_state.update_counter(new_counter);
            delay(Config::Common::ITERATION_DELAY_MILLIS);
            continue;
        }
        if (!safe_state.is_last_digit())
        {
            safe_state.change_direction();
            safe_state.increment_position();
            safe_state.update_counter(new_counter);
            delay(Config::Common::ITERATION_DELAY_MILLIS);
            continue;
        }
        if (safe_state.is_matching_code())
        {
            delay(Config::Common::OPENED_DELAY_MILLIS);
        }
        else
        {
            reset(&safe_state);
            button.reset_is_triggered_flag();
            safe_state.update_counter(new_counter);
        }
    }
}
