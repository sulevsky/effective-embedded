#ifdef DEV_PLATFORM_ARDUINO

#include <Arduino.h>
#include "constants.h"
#define INPUT_PIN GPIO_NUM_8
#define REFERENCE_VOLTAGE_MILLIS 3300
#define RESOLUTION 4096

void setup()
{
    Serial.begin(115200);
    pinMode(INPUT_PIN, INPUT);
}

void loop()
{
    uint32_t raw = analogRead(INPUT_PIN);
    uint32_t mv_calculated = raw * REFERENCE_VOLTAGE_MILLIS / RESOLUTION;
    uint32_t mv_measured = analogReadMilliVolts(INPUT_PIN);
    uint32_t error = mv_calculated - mv_measured;
    printf("Raw: %d, Voltage calculated: %d, Voltage measured: %d, Error: %d\n",  raw, mv_calculated, mv_measured, error);
    delay(TIMESTEP_MILLIS);
}

#endif