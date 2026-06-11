#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <ldr.h>
#include <servo.h>
#include <config.h>

#define SERVO_PERIOD_MS 1000 / SERVO_FREQ
#define SERVO_MAX_DUTY ((1 << SERVO_RESOLUTION) - 1)

extern "C" void app_main(void)
{
    Ldr ldr(LdrConfig::ADC_UNIT, LdrConfig::ADC_CHAN, LdrConfig::ADC_BITWIDTH, LdrConfig::ADC_ATTEN);
    ldr.init();

    Servo servo(ServoConfig::SERVO_TIMER, ServoConfig::SERVO_CHANNEL, ServoConfig::SERVO_RESOLUTION, ServoConfig::SERVO_FREQ, ServoConfig::SERVO_PIN);
    servo.init();

    while (1)
    {
        int ldr_raw = ldr.read_raw();
        int ldr_calibrated_mv = ldr.convert_to_calibrated_mv(ldr_raw);
        int ldr_scaled_digitized = ldr.scale_from_mv_to_digitized(ldr_calibrated_mv);
#ifdef DEBUG
        printf("Raw ADC Value: %d\tVoltage Value: %d mV\n", ldr_raw, ldr_calibrated_mv);
#endif

        servo.set_angle(ldr_scaled_digitized);
        vTaskDelay(pdMS_TO_TICKS(CommonConfig::SUPERLOOP_DELAY));
    }
}
