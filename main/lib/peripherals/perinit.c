#include "includes.h"
#include "perinit.h"

//Analog to Digital Convertion API
#include <driver/adc.h> 

//Digital to Analog conversion API
#include <driver/dac.h> 


////Pulse Width Mmodulation API
#include "driver/ledc.h"


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
//****************************** Injector PWM Setup ***************************************//
//*****************************************************************************************//
//*****************************************************************************************//



void set_injPWM(float freq, float duty)
{   
    ledc_timer_config_t timerConfig;
    timerConfig.duty_resolution =LEDC_TIMER_10_BIT ;
    timerConfig.timer_num = LEDC_TIMER_0;
    timerConfig.freq_hz = ckpPWM*44;
    timerConfig.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_timer_config(&timerConfig);

    ledc_channel_config_t tChaConfig;
    tChaConfig.gpio_num = 0 ;
    tChaConfig.speed_mode = LEDC_HIGH_SPEED_MODE;
    tChaConfig.channel = LEDC_CHANNEL_0;
    tChaConfig.intr_type = LEDC_INTR_DISABLE;
    tChaConfig.timer_sel = LEDC_TIMER_0;
    tChaConfig.duty = TPS; // (2^10)*(%) 
    ledc_channel_config(&tChaConfig);
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
