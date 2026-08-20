#pragma once

#include "stdint.h"
#include "stdint.h"

#define ADC_CALIBRATED_MIN 1050
#define ADC_CALIBRATED_MAX 3978
#define ADC_RANGE ((ADC_CALIBRATED_MAX) - (ADC_CALIBRATED_MIN))

#define CCR_MIN 500
#define CCR_MAX 2500

#define CCR_RANGE ((CCR_MAX) - (CCR_MIN))
#define START_DEGREE 90
#define RANGE_DEGREEES 180

uint32_t reverted_servo_ccr(uint32_t input);
uint32_t into_servo_ccr(uint32_t input);
float into_degrees(uint32_t ccr);
