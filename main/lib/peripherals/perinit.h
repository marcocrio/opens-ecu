#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../includes.h"

// Register WiFi functions
void setADC(); //Set up ADC at 10 bit
void set_injPWM(float freq, float duty); //Sets up PWM
void deb();
void inj_pwm(void *pvParameter);



#ifdef __cplusplus
}
#endif

