#pragma once

#include "driver/gpio.h"
#include "driver/uart.h"
#include "button.h"
#include "led.h"

class Communicator
{
private:
    static const int32_t BAUD_RATE = 115200;
    static const uart_port_t UART = UART_NUM_1;
    static const uint32_t RX_BUFFER_SIZE = 1000;
    static const uint32_t TX_BUFFER_SIZE = 0;
    static const uint32_t TX_PIN_NUM = 17;
    static const uint32_t RX_PIN_NUM = 18;
    static const uint32_t RTS_PIN_NUM = 19;
    static const uint32_t CTS_PIN_NUM = 20;
    static const char OUT_MESSAGE_LENGTH = 1;
    static constexpr char OUT_MESSAGE[OUT_MESSAGE_LENGTH] = {'x'};
    static const char IN_MESSAGE_LENGTH = 1;
    static const char READ_TIMEOUT_MILLIS = 0;
    
    char in_buffer[IN_MESSAGE_LENGTH] = {};
    void send();

public:
    Communicator();
    void init();
    void handle_button_event(Button *button, Led *led);
    void handle_receive_event(Led *led);
};
