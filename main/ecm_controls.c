/* Open source Electronic Control Module
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

//includes
#include "includes.h" // includes all standard & commond libraries accross header files


//console
#include "lib/console/cons.h" //initializates and runs console
#include "cmd_decl.h" // register commands for console


//SPIFFS
#include "lib/spiffsVFS/spiffsVFS.h" //mounts spiffs partition


//peripherals
#include "lib/peripherals/perinit.h"

//Calculations
#include "lib/calculations/calc.h"

const char *TAG = "task";


// ================== Global Variables Definition ==================//
   
SemaphoreHandle_t xMutex;


//-----------------Constans-----------------// 

    const float Vengine= 0.63; //VE Engine Constant (L) 
    const float UniGas= 0.28705; //Universal Gas Constant
    const int cylinder= 1; // **pending change to cylynders
    const float staticFlow= 10; // 10g/ms from Injector Datasheet
    const float openTime= 0.9 ;//The time in takes for the injector to be fully opened


//-----------------Sensors------------------//
    int TPS; //TPS adc readgins 
    float IAT = 311; //Air temperature in kelvin
    float TPSV;//TPS Voltage
    float pressure; //Barometric Pressure (hPa)
    float airmass, fuelmass, RPM, TPS_Percentage;
    float ckpPWM; //Ckp teeth PWM;
    float airmass; 
    float afr=14.7; //airfuel ratio (**needs to be improved, get from tables from book)  

    //Fuel Injector
    float freq;
    float injDuty;
    float injPulseTime;
    float injCycle;


//------------------------DAQ------------------------//

/* Variable holding number of times ESP32 restarted since first boot.
* It is placed into RTC memory using RTC_DATA_ATTR and
* maintains its value when ESP32 wakes from deep sleep.
*/
RTC_DATA_ATTR static int boot_count = 0;



//*****************************************************************************************//
//*****************************************************************************************//
//************************************** MAIN *********************************************//
//*****************************************************************************************//
//*****************************************************************************************//

void app_main(void) 
{
    
//----------------------- NVS init ------------------------//
    

    
    initialize_nvs();

    #if CONFIG_STORE_HISTORY
        initialize_filesystem();
        ESP_LOGI(FS, "Command history enabled");
    #else
        ESP_LOGI(FS, "Command history disabled");
    #endif



//----------------------- Boot count ------------------------//
    
    printf("\n\n");
    ESP_LOGE(SYS, "ECM has booted up");
    ++boot_count;
    ESP_LOGI(SYS, "Boot count: %d\n\n", boot_count);


//---------------------- Peripherals ----------------------//

    
    vfsSetup(); //initializes Virtual File System
    rdfile(); //reads VE table and brings it to registers
    setADC(); //sets up ADC
    initialize_console();// initializes uart and console
    // xMutex = xSemaphoreCreateMutex();




//--------------------Tasks registration--------------------//

    xTaskCreate(&pwm_signals, "pwm_signals", 1024, NULL, 5, NULL); 
    xTaskCreate(&main_Readings, "main_Readings", 2048, NULL, 5, NULL);
    xTaskCreate(&olcmds, "olcmds",4*1024, NULL,5,NULL);
    xTaskCreate(&calc_display, "calc_display", 4*2048, NULL, 5, NULL);




//----------------- Command registration ------------------//

    /* Register commands */
    esp_console_register_help_command();
    // register_system();
    register_file();
    // register_system();

    // consoleRun();



}