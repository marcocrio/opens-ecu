#include "includes.h" //include all standard libraries
#include "../spiffsVFS/spiffsVFS.h" //to use interpolation
#include "../peripherals/perinit.h"

const char *SYS = "SYSTEM";

void main_Readings(void *pvParameter)
{

    dac_output_enable(DAC_CHANNEL_1);//pressure output
    dac_output_enable(DAC_CHANNEL_2);//RPMS output
    int i; 
    xSemaphore = xSemaphoreCreateMutex();

    while(1)
    {
       
        if( xSemaphore != NULL )
        {
           
            if( xSemaphoreTake( xSemaphore, ( TickType_t ) 0 ) )
            {

                // ESP_LOGI(SYS, "Readings:\n");
                
                //get timestamp
                //strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
                //ESP_LOGI(SNTP, "%s\n", strftime_buf);
            
            //Get the read from TPS
                TPS =0; 
            for(i=0 ; i<1000; i++)
                { 
                    TPS += adc1_get_raw(ADC1_CHANNEL_4); 
                } 
                TPS= TPS/ 1000;  
                // printf("TPS ADC: %d \n",TPS);
                vTaskDelay( 2 );
                
                //Get TPS Voltage
                TPSV = TPS * 0.00449658;  
                // printf("TPS Voltage: %.4f (V)\n",TPSV); 

                //Get TPS Percentage
                TPS_Percentage = (TPSV/4.6)*100; //originally voltage/4.6*100 changed to pressure/4.6 
                // printf("TPS%%: %.4f%%\n",TPS_Percentage); 


                //Get MAP Reading on TPS% relation
                if (TPS_Percentage<11)
                pressure = -20*(TPS_Percentage)+1022;
                else
                pressure = -2.15*(TPS_Percentage-20)+755;

                
                
                //Get RPM Based on TPS% relation
                if(TPS_Percentage<10)
                    RPM = 100*(TPS_Percentage)+1200;
                else if (TPS_Percentage > 40) //added for RPMS 
                    RPM= 10*(TPS_Percentage-60)+8000;
                else
                    RPM= 200*(TPS_Percentage-10)+2000; 

                // printf("RPM: %.4f\n",RPM);  


                //Get CKP PWM based on RPM Relation
                if(RPM == 0) 
                    ckpPWM = (60/(44*1))*1000000;
                else 
                    ckpPWM = (60/(44*RPM))*1000000;

                // printf("ckpPWM: %.4f us\n",ckpPWM);

            

                //Volumetric Efficiency 
                interpolation(pressure,RPM); //Gets the exact VE value
                // printf("Volumetric Efficiency: %.2f\n",VE_Value);
                vTaskDelay( 2 );

                //Airmass
                airmass = (Vengine*VE_Value*pressure)/(UniGas*IAT*cylinder); 
                // printf("airmass: %.4f (g/cyl)\n",airmass);

                //Fuelmass
                fuelmass = (airmass)/(afr); 
                // printf("fuelmass: %.4f (g/cyl)\n\n",fuelmass); 


                //Fuel injector
                freq=ckpPWM*44;
                // injCycle = 60/RPM;
                // injPulseTime= (fuelmass/staticFlow + openTime)*1000; //the open time
                injDuty = TPS;

                
                // printf("Frequency: %.4f\n",freq);
                // printf("Injecor Pulse Time: %.4fms\n",injPulseTime);
                // printf("Injector Duty Cyle: %.4f\n\n",injDuty);
            }
            if( xSemaphoreGive( xSemaphore ) != pdTRUE )
            {
                ESP_LOGE(SYS, "xSemaphore failed to get released" );
            }
        }
        vTaskDelay( 2 );
    }
}

//*****************************************************************************************//
//*****************************************************************************************//
//************************************* Display *******************************************//
//*****************************************************************************************//
//*****************************************************************************************//
void calc_display(void *pvParameter){

    
    while(1){

        ESP_LOGI(SYS, "Readings:");

        printf("TPS ADC: %d \n",TPS);
        printf("TPS Voltage: %.4f (V)\n",TPSV); 
        printf("TPS%%: %.4f%%\n",TPS_Percentage); 
        printf("pressure: %.4f (kPa)\n",pressure); 
        printf("RPM: %.4f\n",RPM);
        printf("ckpPWM: %.4f us\n",ckpPWM);
        printf("Volumetric Efficiency: %.2f\n",VE_Value);
        printf("airmass: %.4f (g/cyl)\n",airmass);
        printf("fuelmass: %.4f (g/cyl)\n\n",fuelmass); 
        printf("Frequency: %.4f\n",freq);
        // printf("Injecor Pulse Time: %.4fms\n",injPulseTime);
        printf("Injector Duty Cyle: %.4f\n\n",injDuty);

        


        vTaskDelay( 500 / portTICK_PERIOD_MS);
    }

}