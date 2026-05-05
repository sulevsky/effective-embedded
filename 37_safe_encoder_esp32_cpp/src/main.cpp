#include "driver/gpio.h"
#include "esp_timer.h"
#include "config.h"
#include "time_utils.h"
#include "esp_cpu.h"
#include "encoder.h"
#include "led.h"
#include "driver/gptimer.h"

Led red(Config::Led::RED_GPIO);
Led green(Config::Led::GREEN_GPIO);
Encoder encoder(Config::Encoder::ENCODER_GPIO_A, Config::Encoder::ENCODER_GPIO_B);

extern "C" void app_main(void)
{
    printf("Init..");
    red.init();
    green.init();

    encoder.init();

    while (true)
    {
        int32_t counter = encoder.get_count();
        printf("Counter: %ld\n", counter/4);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
