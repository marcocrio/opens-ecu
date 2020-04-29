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
   
//-----------------LOG Constans----------------//

#define TAG "app"

#define GPIO_SIGNAL_INPUT (34)
#define GPIO_RMT_GATE     (12)
//#define GPIO_LED          (2) //CONFIG_LED_GPIO)

// internal signals for GPIO constant levels
#define GPIO_CONSTANT_LOW   0x30
#define GPIO_CONSTANT_HIGH  0x38

#define PCNT_UNIT         (0)
#define PCNT_CHANNEL      (PCNT_CHANNEL_0)
#define RMT_CHANNEL       (RMT_CHANNEL_0)
#define RMT_MAX_BLOCKS    (2)   // allow up to 2 * 64 * (2 * 32767) RMT periods in window
#define RMT_CLK_DIV       160   // results in 2us steps (80MHz / 160 = 0.5 MHz
//#define RMT_CLK_DIV       20    // results in 0.25us steps (80MHz / 20 = 4 MHz
//#define RMT_CLK_DIV       1     // results in 25ns steps (80MHz / 2 / 1 = 40 MHz)

#define SAMPLE_PERIOD 1  // seconds

// The counter is signed 16-bit, so maximum positive value is 32767
// The filter is unsigned 10-bit, maximum value is 1023. Use full period of maximum frequency.
// For higher expected frequencies, the sample period and filter must be reduced.

// suitable up to 16,383.5 Hz
#define WINDOW_DURATION 1  // seconds
#define FILTER_LENGTH 1023  // APB @ 80MHz, limits to < 39,100 Hz

static void window_start_callback(void)
{
    ESP_LOGI(TAG, "Begin sampling");
    gpio_matrix_in(GPIO_SIGNAL_INPUT, SIG_IN_FUNC228_IDX, false);
}

static void frequency_callback(double hz)
{
    gpio_matrix_in(GPIO_CONSTANT_LOW, SIG_IN_FUNC228_IDX, false);
    ESP_LOGI(TAG, "Frequency %f Hz", hz);
}

// static void config_led(void)
// {
//     gpio_pad_select_gpio(GPIO_LED);
//     gpio_set_direction(GPIO_LED, GPIO_MODE_OUTPUT);

//     // route incoming frequency signal to onboard LED when sampling
//     gpio_matrix_out(GPIO_LED, SIG_IN_FUNC228_IDX, false, false);
// }
//-----------------Constans-----------------// 

    const float Vengine= 0.63; //VE Engine Constant (L) 
    const float UniGas= 0.28705; //Universal Gas Constant
    const int cylinder= 1; // **pending change to cylynders
    const float staticFlow= 10; // 10g/ms from Injector Datasheet
    const float openTime= 0.9 ;//The time in takes for the injector to be fully opened
    // const int atmospheric = 101; // Standard atmospheric pressure in kpa
    // const float MFM = 0.039; // maximum fuel mass


//-----------------Sensors------------------//
    int TPS; //TPS adc readgins 
    // int TPS_ADC;//TPS ADC Value
    float IAT = 311; //Air temperature in kelvin
    float TPSV;//TPS Voltage
    float pressure; //Barometric Pressure (hPa)
    float airmass, fuelmass, RPM, TPS_Percentage;
    float ckpPWM; //Ckp teeth PWM;
    float airmass; 
    float afr=14.7; //airfuel ratio (**needs to be improved, get from tables from book)  

    //Fuel Injector
    float freq = 1;
    float injDuty;
    float injPulseTime;
    float injCycle;

    //Buffer
    float readings_buff[9];
    int flag = 1;

    //Frequency
    double frequency_hz;

    

//------------------------DAQ------------------------//

/* Variable holding number of times ESP32 restarted since first boot.
* It is placed into RTC memory using RTC_DATA_ATTR and
* maintains its value when ESP32 wakes from deep sleep.
*/
RTC_DATA_ATTR static int boot_count = 0;








static int tasks_info()
{
    const size_t bytes_per_task = 40; /* see vTaskList description */
    char *task_list_buffer = malloc(uxTaskGetNumberOfTasks() * bytes_per_task);
    if (task_list_buffer == NULL) {
        ESP_LOGE(TAG, "failed to allocate buffer for vTaskList output");
        return 1;
    }
    fputs("Task Name\tStatus\tPrio\tHWM\tTask#", stdout);
#ifdef CONFIG_FREERTOS_VTASKLIST_INCLUDE_COREID
    fputs("\tAffinity", stdout);
#endif
    fputs("\n", stdout);
    vTaskList(task_list_buffer);
    fputs(task_list_buffer, stdout);
    free(task_list_buffer);
    return 0;
}


//*****************************************************************************************//
//*****************************************************************************************//
//************************************** MAIN *********************************************//
//*****************************************************************************************//
//*****************************************************************************************//

void app_main(void) 
{
    
//----------------------- Boot count ------------------------//
    
    printf("\n\n");
    ESP_LOGE(SYS, "ECM has booted up");
    ++boot_count;
    ESP_LOGI(SYS, "Boot count: %d\n\n", boot_count);

    tasks_info();


//---------------------- Peripherals ----------------------//
    frequency_count_configuration_t * config = malloc(sizeof(*config));
    config->pcnt_gpio = GPIO_SIGNAL_INPUT;
    config->pcnt_unit = PCNT_UNIT;
    config->pcnt_channel = PCNT_CHANNEL;
    config->rmt_gpio = GPIO_RMT_GATE;
    config->rmt_channel = RMT_CHANNEL;
    config->rmt_clk_div = RMT_CLK_DIV;
    config->rmt_max_blocks = 2;
    config->sampling_period_seconds = SAMPLE_PERIOD;
    config->sampling_window_seconds = WINDOW_DURATION;
    config->filter_length = FILTER_LENGTH;
    config->window_start_callback = &window_start_callback;
    config->frequency_update_callback = &frequency_callback;
    
    vfsSetup(); //initializes Virtual File System
    // esp_task_wdt_init(30,0);// Watchdog timer settings. it lasts 30 seconds and the 0 indicates that there will not be error.
    rdfile();
    setADC();

//--------------------Tasks registration--------------------//

    xTaskCreate(&pwm_signals, "pwm_signals", 2048, NULL, 4, NULL); 
    xTaskCreate(&main_Readings, "main_Readings", 2048, NULL, 5, NULL);
    xTaskCreate(&calc_display, "calc_display", 2048, NULL, 4, NULL);
    //xTaskCreate(&deb, "pwm_debugging", 2048, NULL, 4, NULL);
    //xTaskCreate(&motor_signal, "motor_signal", 2048, NULL, 4, NULL);
    xTaskCreate(&frequency_count_task_function,"frequency_count_task",4096,config,4,NULL);
    
//----------------------- NVS init ------------------------//
    

    
    initialize_nvs();

    #if CONFIG_STORE_HISTORY
        initialize_filesystem();
        ESP_LOGI(FS, "Command history enabled");
    #else
        ESP_LOGI(FS, "Command history disabled");
    #endif


//----------------- Command registration ------------------//

    initialize_console();

    /* Register commands */
    esp_console_register_help_command();
    // register_system();
    register_file();
    // register_system();

    // consoleRun();


}