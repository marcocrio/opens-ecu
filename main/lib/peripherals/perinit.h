#pragma once

#ifdef __cplusplus
extern "C" {
#endif


// Register WiFi functions
void setADC(); //Set up ADC at 10 bit
void set_injPWM(float freq, float duty); //Sets up PWM
void deb();



#ifdef __cplusplus
}
#endif

