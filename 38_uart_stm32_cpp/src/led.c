#include "config.h"
#include "led.h"
#include "stm32f4xx_hal.h"

void toggle_local_led()
{
    HAL_GPIO_TogglePin(LOCAL_LED_PORT, LOCAL_LED_PIN);
}

void toggle_remote_led()
{
    HAL_GPIO_TogglePin(REMOTE_LED_PORT, REMOTE_LED_PIN);
}
