// not used, added to recreate example from the lesson

#ifndef DEV_PLATFORM_ESP_IDF

#include <Arduino.h>
#include "constants.h"

#define BAUDRATE (115200)
#define BUTTON (4)

volatile uint32_t counter = 0;

void toggleLED()
{
    counter++;
}

void setup()
{
    // Experiment 1
    // pinMode(BUTTON, INPUT);
    // Experiment 2
    pinMode(BUTTON, INPUT_PULLUP);
    Serial.begin(BAUDRATE);

    attachInterrupt(digitalPinToInterrupt(BUTTON), toggleLED, FALLING);
}

void loop()
{
    static uint32_t counter_current = 0;

    if (counter != counter_current)
    {
        Serial.println("Counter: " + String(counter));
        counter_current = counter;
    }
}

#endif
