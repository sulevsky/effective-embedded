#include "config.h"
#include "time_utils.h"
#include "button.h"
#include "led.h"
#include "communicator.h"

extern "C" void app_main(void)
{
    delay(Config::Common::LOGGING_UART_INIT_DELAY_MILLIS);
    printf("Init..\n");

    Button button(Config::Button::BUTTON_GPIO);
    button.init();

    Led remote_controlled_led(Config::Led::REMOTE_LED_GPIO);
    remote_controlled_led.init();
    Led local_controlled_led(Config::Led::LOCAL_LED_GPIO);
    local_controlled_led.init();

    Communicator communicator;
    communicator.init();

    while (true)
    {
        communicator.handle_button_event(&button, &local_controlled_led);
        communicator.handle_receive_event(&remote_controlled_led);
        delay(10);
    }
}
