//standard C
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "sdkconfig.h"

//standard freeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "freertos/FreeRTOSConfig.h"


//standard esp
#include "esp_int_wdt.h"    //watchdog timer
#include "esp_task_wdt.h"   //watchdog timer
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"


//console
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "cmd_decl.h"

//calc & peripherals

#include <driver/adc.h> //Analog to Digital Convertion API
#include <driver/dac.h> //Digital to Analog conversion API 
#include "driver/ledc.h" //Pulse Width Mmodulation API


//-----------------Constans-----------------// 

    extern const float Vengine; //VE Engine Constant ()
    extern const float UniGas; //Universal Gas Constant
    extern const int cylinder; // **pending change to cylynders
    extern const float staticFlow; // 10g/ms from Injector Datasheet
    extern const float openTime;//The time in takes for the injector to be fully opened


//-----------------Sensors------------------//
    extern int TPS; //TPS adc readgins 
    extern float IAT; //Air temperature in kelvin
    extern float TPSV; //TPS Voltage
    extern float pressure; //Barometric Pressure (hPa)
    extern float airmass, fuelmass, RPM, TPS_Percentage;
    extern float ckpPWM; //Ckp teeth PWM;
    extern float airmass; 
    extern float afr; //airfuel ratio (**needs to be improved, get from tables from book)  

    //Fuel Injector
    extern float freq;
    extern float injDuty;
    extern float injPulseTime;
    extern float injCycle;

    //Buffer
    extern float readings_buff[9];
    #define TPSb 0
    #define CKPb 1
    #define MAPb 2
    #define RPMb 3
    #define Freqb 4
    #define VEb  5
    #define Airmassb 6
    #define Fuelmassb 7
    #define DutyCicleb 8
    
    extern int flag;