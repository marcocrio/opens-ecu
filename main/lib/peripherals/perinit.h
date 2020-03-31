#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../includes.h"

// Register WiFi functions
void setADC(); //Set up ADC at 10 bit
void set_injPWM(float freq, float duty); //Sets up PWM
void deb();
void pwm_signals(void *pvParameter);
void perinit_display(void *pvParameter);



#ifdef __cplusplus
}
#endif

