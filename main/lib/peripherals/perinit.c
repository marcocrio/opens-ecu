#include "includes.h"
#include "perinit.h"

//-----------------LOG Constans----------------//




//*****************************************************************************************//
//*****************************************************************************************//
//************************************* ADC Setup *****************************************//
//*****************************************************************************************//
//*****************************************************************************************//

void setADC() 
{  
    adc1_config_width(ADC_WIDTH_BIT_10); 
    adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_11); 
} 



//*****************************************************************************************//
//*****************************************************************************************//
//********************************** Injector PWM *****************************************//
//*****************************************************************************************//
//*****************************************************************************************//

void pwm_signals(void *pvParameter) // square wave signals for injector and spark plug
{    
    
    gpio_pad_select_gpio(4);
    gpio_pad_select_gpio(16);
    esp_task_wdt_add(NULL);// subscription to WDT

    /* Set the GPIO as a push/pull output */
    gpio_set_direction(4,GPIO_MODE_OUTPUT);
    gpio_set_direction(16,GPIO_MODE_OUTPUT);

        int freq = 1; // frequency in hz
        float dutyper = 70; // duty cycle in percentage
        float tduty = dutyper/100 * 1000000; // duty cycle for calculations 
        float duty_inv_per = 100 - dutyper; // counter part of duty cycle percentage
        float duty_inv = duty_inv_per/100 * 1000000; // counter part of duty cycle for calculations
        int delay = (int)tduty;
        int delay_inv = (int)duty_inv;

    while(1) 
    {
        gpio_set_level(2, 1);
        gpio_set_level(16, 0);
        ets_delay_us(delay/freq);
       
        gpio_set_level(2, 0);
        gpio_set_level(16, 1);
        ets_delay_us(delay_inv/freq);
        
        vTaskDelay(2);
        esp_task_wdt_reset();
    }
};

//*****************************************************************************************//
//*****************************************************************************************//
//************************************* Debbug ********************************************//
//*****************************************************************************************//
//*****************************************************************************************//


void deb(){

    
    TPS= 5;
    printf("\nTPS: %d\nVengine: %.2f\n\n",TPS,Vengine);


};
