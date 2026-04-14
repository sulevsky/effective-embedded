#include "time_utils.h"
#include "stdint.h"
#include "esp_timer.h"
#include "config.h"
#include "freertos/FreeRTOS.h"

bool is_expired(uint32_t now, uint32_t last_event, uint32_t timeout)
{
    return (now - last_event) >= timeout;
}

uint32_t now_millis()
{

    return (uint32_t)(esp_timer_get_time() / CommonConfig::MILLIS_IN_MICROS);
}

uint64_t now_micros()
{

    return (uint64_t)esp_timer_get_time();
}

void delay(uint32_t millis)
{
    vTaskDelay(pdMS_TO_TICKS(millis));
}
