#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "esp_timer.h"
#include "config.h"
#include "time_utils.h"
#include "esp_cpu.h"
#include "encoder.h"
#include "button.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define UART_PORT_NUM (2)

extern "C" void app_main(void)
{
    delay(Config::Common::UART_INIT_DELAY_MILLIS);
    printf("Init..\n");

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
    uart_set_pin(UART_NUM_1, 17, 18, 19, 20);

    uint8_t buffer[1] = {};
    while (true)
    {
        // uart_read_bytes();
        // uart_write_bytes(UART_NUM_1, "x", 1);
        // uart_write_bytes(UART_NUM_1, "\r\n", 2);
        // printf("Init..\n");
        int len = uart_read_bytes(UART_NUM_1, buffer, 1, 0);
        if (len)
        {

            char out[1] = {buffer[0]};
            uart_write_bytes(UART_NUM_1, out, 1);
        }
        // delay(1000);
    }
}
