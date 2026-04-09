#include "driver/gpio.h"
#include "esp_timer.h"
#include "led.h"
#include "ldr.h"
#include "config.h"
#include "time_utils.h"
#include "esp_adc/adc_oneshot.h"

extern "C" void app_main(void)
{
    printf("Init...\n");
    Ldr ldr(LdrConfig::LDR_ADC_UNIT, LdrConfig::LDR_ADC_CHAN);
    ldr.init();

    MaSmoothedLdr ma_smoothed_ldr(&ldr);

    Led led(LedConfig::LED_GPIO);
    led.init();
    
    uint32_t last_ldr_read = 0;
    uint32_t last_log = 0;

    while (true)
    {
        uint32_t now = (uint32_t)(esp_timer_get_time() / CommonConfig::MILLIS_IN_MICROS);
        if (is_expired(now, last_ldr_read, LdrConfig::READ_PERIOD_MS))
        {
            int smoothed_ldr_data = ma_smoothed_ldr.read_smoothed();
            led.set_state(Ldr::RESOLUTION_MAX_VALUE - smoothed_ldr_data);
            last_ldr_read = now;
        }
        if (is_expired(now, last_log, CommonConfig::LOG_PERIOD_MS))
        {
            int raw = ldr.read_raw();
            int calibrated = ldr.read_calibrated();
            int smoothed = ma_smoothed_ldr.read_smoothed();
            printf("Raw: %d\n", raw);
            printf("Calibrated: %d\n", calibrated);
            printf("Smoothed: %d\n", smoothed);
            last_log = now;
        }
    }
}
