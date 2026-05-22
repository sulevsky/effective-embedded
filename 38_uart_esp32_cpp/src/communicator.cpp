#include "driver/gpio.h"
#include "config.h"
#include "communicator.h"
#include "button.h"
#include "led.h"
#include "time_utils.h"

Communicator::Communicator()
{
}

void Communicator::init()
{
    uart_config_t uart_config = {
        .baud_rate = BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    uart_driver_install(UART, RX_BUFFER_SIZE, TX_BUFFER_SIZE, 0, NULL, 0);
    uart_param_config(UART, &uart_config);
    uart_set_pin(UART, TX_PIN_NUM, RX_PIN_NUM, RTS_PIN_NUM, CTS_PIN_NUM);
}

void Communicator::send()
{
    uart_write_bytes(UART, OUT_MESSAGE, OUT_MESSAGE_LENGTH);
    printf("sent...\n");
}

void Communicator::handle_button_event(Button *button, Led *led)
{
    static uint64_t last_registered_time = 0;
    if (button->is_triggered())
    {
        auto now = now_micros();
        if (is_expired(now, last_registered_time, Config::Button::DEFAULT_DEBOUNCE_DELAY_MICROS))
        {
            last_registered_time = now;
            send();
            led->toggle();
        }
        button->reset_is_triggered_flag();
    }
};

void Communicator::handle_receive_event(Led *led)
{
    int len = uart_read_bytes(UART, in_buffer, IN_MESSAGE_LENGTH, READ_TIMEOUT_MILLIS);
    if (len && in_buffer[0])
    {
        led->toggle();
        printf("received data: %c\n", in_buffer[0]);
    }
}
