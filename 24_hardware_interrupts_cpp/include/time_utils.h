#pragma once

#include "stdint.h"
#include "freertos/FreeRTOS.h"
#include "esp_timer.h"

inline bool is_expired(const uint32_t now, const uint32_t last_event, const uint32_t timeout)
{
    return (now - last_event) >= timeout;
}

inline uint32_t now_millis()
{

    return (uint32_t)(esp_timer_get_time() / Config::Common::MILLIS_IN_SECONDS);
}

inline uint64_t now_micros()
{

    return (uint64_t)esp_timer_get_time();
}

inline void delay(uint32_t millis)
{
    vTaskDelay(pdMS_TO_TICKS(millis));
}
