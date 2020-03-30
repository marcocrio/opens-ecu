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



void set_injPWM(float freq, float duty)
{   
    ledc_timer_config_t timerConfig;
    timerConfig.duty_resolution =LEDC_TIMER_5_BIT ;
    timerConfig.timer_num = LEDC_TIMER_0;
    timerConfig.freq_hz = freq;
    timerConfig.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_timer_config(&timerConfig);

    ledc_channel_config_t tChaConfig;
    tChaConfig.gpio_num = 0 ;
    tChaConfig.speed_mode = LEDC_HIGH_SPEED_MODE;
    tChaConfig.channel = LEDC_CHANNEL_0;
    tChaConfig.intr_type = LEDC_INTR_DISABLE;
    tChaConfig.timer_sel = LEDC_TIMER_0;
    tChaConfig.duty = duty; // (2^10)*(%) 
    ledc_channel_config(&tChaConfig);
}


void inj_pwm(void *pvParameter){

    while(1){

        ledc_set_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_0, 0);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_0);
    }


}

//*****************************************************************************************//
//*****************************************************************************************//
//************************************* Debbug ********************************************//
//*****************************************************************************************//
//*****************************************************************************************//


void deb(){

    
    TPS= 5;
    printf("\nTPS: %d\nVengine: %.2f\n\n",TPS,Vengine);


};
