#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "esp_timer.h"
#include "config.h"
#include "time_utils.h"
#include "esp_cpu.h"
#include "button.h"
#include "led.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define UART_PORT_NUM (2)

void send()
{
    char out[] = {'x'};
    printf("sent..\n");
    uart_write_bytes(UART_NUM_1, out, 1);
}

void handle_button_event(Button *button)
{
    static uint64_t last_registered_time = 0;
    if (button->is_triggered())
    {
        auto now = now_micros();
        if (is_expired(now, last_registered_time, Config::Button::DEFAULT_DEBOUNCE_DELAY_MICROS))
        {
            last_registered_time = now;
            send();
        }
        button->reset_is_triggered_flag();
    }
};

void handle_receive_event(Led *led)
{
    uint8_t buffer[1] = {};
    int len = uart_read_bytes(UART_NUM_1, buffer, 1, 0);
    if (len && buffer[0])
    {
        printf("received data: %c\n", buffer[0]);
        led->toggle();
    }
}

extern "C" void app_main(void)
{
    delay(Config::Common::UART_INIT_DELAY_MILLIS);
    printf("Init..\n");

    Button button(Config::Button::BUTTON_GPIO);
    button.init();

    Led led(Config::Led::LED_GPIO);
    led.init();

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;
    uart_driver_install(UART_NUM_1, 1000, 0, 0, NULL, intr_alloc_flags);
    uart_param_config(UART_NUM_1, &uart_config);
    auto r = uart_set_pin(UART_NUM_1, 17, 18, 19, 20);
    printf("r is %d\n", r);
    while (true)
    {
        handle_button_event(&button);
        handle_receive_event(&led);
        delay(10);
    }
}
