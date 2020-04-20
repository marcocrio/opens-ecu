#include "includes.h"
#include "perinit.h"

#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

#ifdef INCLUDE_ESP_IDF_VERSION_H
# include "esp_idf_version.h"
#endif

#define TAG "frequency_counter"

// In ESP-IDF v4.1-beta1 (and presumably newer), the macro RMT_MEM_BLOCK_BYTE_NUM has been removed
#if (ESP_IDF_VERSION_MAJOR == 4)
# define RMT_MEM_BLOCK_BYTE_NUM ((SOC_RMT_CHANNEL_MEM_WORDS) * 4)
#endif


// #define PCNT_TEST_UNIT      PCNT_UNIT_0
// #define PCNT_H_LIM_VAL      10
// #define PCNT_L_LIM_VAL     -10
// #define PCNT_THRESH1_VAL    5
// #define PCNT_THRESH0_VAL   -5
// #define PCNT_INPUT_SIG_IO   14  // Pulse Input GPIO
// #define PCNT_INPUT_CTRL_IO  12  // Control GPIO HIGH=count up, LOW=count down
// #define LEDC_OUTPUT_IO      18 // Output GPIO of a sample 1 Hz pulse generator

// xQueueHandle pcnt_evt_queue;   // A queue to handle pulse counter events
// pcnt_isr_handle_t user_isr_handle = NULL; //user's ISR service handle

// // /* A sample structure to pass events from the PCNT
// //  * interrupt handler to the main program.
// //  */
// typedef struct {
//     int unit;  // the PCNT unit that originated an interrupt
//     uint32_t status; // information on the event type that caused the interrupt
// } pcnt_evt_t;

//-----------------LOG Constans----------------//

// push confirmation

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

    
    gpio_pad_select_gpio(2);
    gpio_pad_select_gpio(16);
 
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(2,GPIO_MODE_OUTPUT);
    gpio_set_direction(16,GPIO_MODE_OUTPUT);
 
        int frequency;// = freq; // frequency in hz
        float dutyper;// = 70; // duty cycle in percentage
        float tduty;// = dutyper/100 * 1000000; // duty cycle for calculations 
        float duty_inv_per;// = 100 - dutyper; // counter part of duty cycle percentage
        float duty_inv;// = duty_inv_per/100 * 1000000; // counter part of duty cycle for calculations
        int delay;// = (int)tduty;
        int delay_inv;// = (int)duty_inv;
 
    while(1) 
    {
        TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
        TIMERG0.wdt_feed=1;
        TIMERG0.wdt_wprotect=0;
        if(frequency_hz == 0)
        {
            frequency_hz = 1;
        }

        frequency = (int)frequency_hz;
        dutyper = injDuty;
        tduty = dutyper/100*1000000;
        duty_inv_per = 100 - dutyper;
        duty_inv = duty_inv_per/100 * 1000000;
        delay = (int)tduty;
        delay_inv = (int)duty_inv;


        gpio_set_level(2, 1);
        gpio_set_level(16, 0);
        ets_delay_us(delay/frequency);
       
       
        gpio_set_level(2, 0);
        gpio_set_level(16, 1);
        ets_delay_us(delay_inv/frequency);
        
        //printf("\n ------- Test_Frequency = %d -------- \n",frequency);
        //vTaskDelay(10/portTICK_PERIOD_MS);
    }
};

//*****************************************************************************************//
//*****************************************************************************************//
//************************************* Debbug ********************************************//
//*****************************************************************************************//
//*****************************************************************************************//

void deb(){

    while(1)
    {
    // TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    // TIMERG0.wdt_feed=1;
    // TIMERG0.wdt_wprotect=0;
    // TPS= 5;
    printf("\n --------------------------\nFrequency PWM signal: %.2f\nDuty PWM signal: %.2f\n\n",readings_buff[Freqb],readings_buff[DutyCicleb]);

    vTaskDelay(500/portTICK_PERIOD_MS);
    }


};

//*****************************************************************************************//
//*****************************************************************************************//
//************************************* Pulse Counter ********************************************//
//*****************************************************************************************//
//*****************************************************************************************//

// void IRAM_ATTR pcnt_example_intr_handler(void *arg)
// {
//     uint32_t intr_status = PCNT.int_st.val;
//     int i;
//     pcnt_evt_t evt;
//     portBASE_TYPE HPTaskAwoken = pdFALSE;

//     for (i = 0; i < PCNT_UNIT_MAX; i++) {
//         if (intr_status & (BIT(i))) {
//             evt.unit = i;
//             /* Save the PCNT event type that caused an interrupt
//                to pass it to the main program */
//             evt.status = PCNT.status_unit[i].val;
//             PCNT.int_clr.val = BIT(i);
//             xQueueSendFromISR(pcnt_evt_queue, &evt, &HPTaskAwoken);
//             if (HPTaskAwoken == pdTRUE) {
//                 portYIELD_FROM_ISR();
//             }
//         }
//     }
// }

// void pcnt_example_init(void)
// {
//     /* Prepare configuration for the PCNT unit */
//     pcnt_config_t pcnt_config = {
//         // Set PCNT input signal and control GPIOs
//         .pulse_gpio_num = PCNT_INPUT_SIG_IO,
//         .ctrl_gpio_num = PCNT_INPUT_CTRL_IO,
//         .channel = PCNT_CHANNEL_0,
//         .unit = PCNT_TEST_UNIT,
//         // What to do on the positive / negative edge of pulse input?
//         .pos_mode = PCNT_COUNT_INC,   // Count up on the positive edge
//         .neg_mode = PCNT_COUNT_DIS,   // Keep the counter value on the negative edge
//         // What to do when control input is low or high?
//         .lctrl_mode = PCNT_MODE_REVERSE, // Reverse counting direction if low
//         .hctrl_mode = PCNT_MODE_KEEP,    // Keep the primary counter mode if high
//         // Set the maximum and minimum limit values to watch
//         .counter_h_lim = PCNT_H_LIM_VAL,
//         .counter_l_lim = PCNT_L_LIM_VAL,
//     };
//     /* Initialize PCNT unit */
//     pcnt_unit_config(&pcnt_config);

//     /* Configure and enable the input filter */
//     pcnt_set_filter_value(PCNT_TEST_UNIT, 100);
//     pcnt_filter_enable(PCNT_TEST_UNIT);

//     /* Set threshold 0 and 1 values and enable events to watch */
//     pcnt_set_event_value(PCNT_TEST_UNIT, PCNT_EVT_THRES_1, PCNT_THRESH1_VAL);
//     pcnt_event_enable(PCNT_TEST_UNIT, PCNT_EVT_THRES_1);
//     pcnt_set_event_value(PCNT_TEST_UNIT, PCNT_EVT_THRES_0, PCNT_THRESH0_VAL);
//     pcnt_event_enable(PCNT_TEST_UNIT, PCNT_EVT_THRES_0);
//     /* Enable events on zero, maximum and minimum limit values */
//     pcnt_event_enable(PCNT_TEST_UNIT, PCNT_EVT_ZERO);
//     pcnt_event_enable(PCNT_TEST_UNIT, PCNT_EVT_H_LIM);
//     pcnt_event_enable(PCNT_TEST_UNIT, PCNT_EVT_L_LIM);

//     /* Initialize PCNT's counter */
//     pcnt_counter_pause(PCNT_TEST_UNIT);
//     pcnt_counter_clear(PCNT_TEST_UNIT);

//     /* Register ISR handler and enable interrupts for PCNT unit */
//     pcnt_isr_register(pcnt_example_intr_handler, NULL, 0, &user_isr_handle);
//     pcnt_intr_enable(PCNT_TEST_UNIT);

//     /* Everything is set up, now go to counting */
//     pcnt_counter_resume(PCNT_TEST_UNIT);
// }

// void PulseCounter(void *pvParameter)
// {

//     /* Initialize PCNT event queue and PCNT functions */
//     pcnt_evt_queue = xQueueCreate(10, sizeof(pcnt_evt_t));
//     pcnt_example_init();

//     int16_t count = 0;
//     pcnt_evt_t evt;
//     portBASE_TYPE res;
//     while (1) {
//         /* Wait for the event information passed from PCNT's interrupt handler.
//          * Once received, decode the event type and print it on the serial monitor.
//          */
//         // res = xQueueReceive(pcnt_evt_queue, &evt, 1000 / portTICK_PERIOD_MS);
//         // if (res == pdTRUE) {
//         //     pcnt_get_counter_value(PCNT_TEST_UNIT, &count);
//         //     printf("Event PCNT unit[%d]; cnt: %d\n", evt.unit, count);
//         //     if (evt.status & PCNT_EVT_THRES_1) {
//         //         printf("THRES1 EVT\n");
//         //     }
//         //     if (evt.status & PCNT_EVT_THRES_0) {
//         //         printf("THRES0 EVT\n");
//         //     }
//         //     if (evt.status & PCNT_EVT_L_LIM) {
//         //         printf("L_LIM EVT\n");
//         //     }
//         //     if (evt.status & PCNT_EVT_H_LIM) {
//         //         printf("H_LIM EVT\n");
//         //     }
//         //     if (evt.status & PCNT_EVT_ZERO) {
//         //         printf("ZERO EVT\n");
//         //     }
//         // } else {
//             pcnt_get_counter_value(PCNT_TEST_UNIT, &count);
//             printf("\n-----------------\nCurrent counter value :%d\n-----------------\n", count);
//             vTaskDelay(500/portTICK_PERIOD_MS);
//     //     }
//     // }
//     if(user_isr_handle) {
//         //Free the ISR service handle.
//         esp_intr_free(user_isr_handle);
//         user_isr_handle = NULL;
//     }
//     }
//     vTaskDelay(500/portTICK_PERIOD_MS);
//     //vTaskDelay(2);
    
// }

//*****************************************************************************************//
//*****************************************************************************************//
//************************************* Frequency Counter ********************************************//
//*****************************************************************************************//
//*****************************************************************************************//

void init_rmt(uint8_t tx_gpio, rmt_channel_t channel, uint8_t clk_div)
{
    ESP_LOGD(TAG, "%s", __FUNCTION__);

    rmt_config_t rmt_tx = {
        .rmt_mode = RMT_MODE_TX,
        .channel = channel,
        .gpio_num = tx_gpio,
        .mem_block_num = 1,  // single block
        .clk_div = clk_div,
        .tx_config.loop_en = false,
        .tx_config.carrier_en = false,
        .tx_config.idle_level = RMT_IDLE_LEVEL_LOW,
        .tx_config.idle_output_en = true,
    };
    ESP_ERROR_CHECK(rmt_config(&rmt_tx));
    ESP_ERROR_CHECK(rmt_driver_install(rmt_tx.channel, 0, 0));
}

int create_rmt_window(rmt_item32_t * items, double sampling_window_seconds, double rmt_period)
{
    ESP_LOGD(TAG, "%s", __FUNCTION__);

    int num_items = 0;

    // enable counter for exactly x seconds:
    int32_t total_duration = (uint32_t)(sampling_window_seconds / rmt_period);
    ESP_LOGD(TAG, "total_duration %f seconds = %d * %g seconds", sampling_window_seconds, total_duration, rmt_period);

    // max duration per item is 2^15-1 = 32767
    while (total_duration > 0)
    {
        uint32_t duration = total_duration > 32767 ? 32767 : total_duration;
        items[num_items].level0 = 1;
        items[num_items].duration0 = duration;
        total_duration -= duration;
        ESP_LOGD(TAG, "duration %d", duration);

        if (total_duration > 0)
        {
            uint32_t duration = total_duration > 32767 ? 32767 : total_duration;
            items[num_items].level1 = 1;
            items[num_items].duration1 = duration;
            total_duration -= duration;
        }
        else
        {
            items[num_items].level1 = 0;
            items[num_items].duration1 = 0;
        }
        ESP_LOGD(TAG, "[%d].level0 %d", num_items, items[num_items].level0);
        ESP_LOGD(TAG, "[%d].duration0 %d", num_items, items[num_items].duration0);
        ESP_LOGD(TAG, "[%d].level1 %d", num_items, items[num_items].level1);
        ESP_LOGD(TAG, "[%d].duration1 %d", num_items, items[num_items].duration1);

        ++num_items;
    }
    ESP_LOGD(TAG, "num_items %d", num_items);
    return num_items;
}

void init_pcnt(uint8_t pulse_gpio, uint8_t ctrl_gpio, pcnt_unit_t unit, pcnt_channel_t channel, uint16_t filter_length)
{
    ESP_LOGD(TAG, "%s", __FUNCTION__);

    // set up counter
    pcnt_config_t pcnt_config = {
        .pulse_gpio_num = pulse_gpio,
        .ctrl_gpio_num = ctrl_gpio,
        .lctrl_mode = PCNT_MODE_DISABLE,
        .hctrl_mode = PCNT_MODE_KEEP,
        .pos_mode = PCNT_COUNT_INC,  // count both rising and falling edges
        .neg_mode = PCNT_COUNT_INC,
        .counter_h_lim = 0,
        .counter_l_lim = 0,
        .unit = unit,
        .channel = channel,
    };

    ESP_ERROR_CHECK(pcnt_unit_config(&pcnt_config));

    // set the GPIO back to high-impedance, as pcnt_unit_config sets it as pull-up
    ESP_ERROR_CHECK(gpio_set_pull_mode(pulse_gpio, GPIO_FLOATING));

    // enable counter filter - at 80MHz APB CLK, 1000 pulses is max 80,000 Hz, so ignore pulses less than 12.5 us.
    ESP_ERROR_CHECK(pcnt_set_filter_value(unit, filter_length));
    ESP_ERROR_CHECK(pcnt_filter_enable(unit));
}

void frequency_count_task_function(void * pvParameter)
{
    frequency_count_configuration_t configuration;

    assert(pvParameter);
    ESP_LOGI(TAG, "Core ID %d", xPortGetCoreID());

    configuration = *(frequency_count_configuration_t*)pvParameter;
    frequency_count_configuration_t *task_inputs = &configuration;

    ESP_LOGI(TAG, "pcnt_gpio %d, pcnt_unit %d, pcnt_channel %d, rmt_gpio %d, rmt_clk_div %d, sampling_period_seconds %f, sampling_window_seconds %f, filter_length %d",
        task_inputs->pcnt_gpio,
        task_inputs->pcnt_unit,
        task_inputs->pcnt_channel,
        task_inputs->rmt_gpio,
        task_inputs->rmt_clk_div,
        task_inputs->sampling_period_seconds,
        task_inputs->sampling_window_seconds,
        task_inputs->filter_length);

    init_rmt(task_inputs->rmt_gpio, task_inputs->rmt_channel, task_inputs->rmt_clk_div);
    init_pcnt(task_inputs->pcnt_gpio, task_inputs->rmt_gpio, task_inputs->pcnt_unit, task_inputs->pcnt_channel, task_inputs->filter_length);

    // assuming 80MHz APB clock
    const double rmt_period = (double)(task_inputs->rmt_clk_div) / 80000000.0;

    const size_t items_size = RMT_MEM_BLOCK_BYTE_NUM * task_inputs->rmt_max_blocks;
    rmt_item32_t * rmt_items = malloc(items_size);
    assert(rmt_items);
    memset(rmt_items, 0, items_size);
    int num_rmt_items = create_rmt_window(rmt_items, task_inputs->sampling_window_seconds, rmt_period);
    assert(num_rmt_items <= task_inputs->rmt_max_blocks * RMT_MEM_ITEM_NUM);

    TickType_t last_wake_time = xTaskGetTickCount();
    

    while (1)
    {
        last_wake_time = xTaskGetTickCount();

        // clear counter
        ESP_ERROR_CHECK(pcnt_counter_clear(task_inputs->pcnt_unit));

        // start sampling window
        ESP_ERROR_CHECK(rmt_write_items(task_inputs->rmt_channel, rmt_items, num_rmt_items, false));

        // call wndow-start callback if set
        if (task_inputs->window_start_callback)
        {
            (task_inputs->window_start_callback)();
        }

        // wait for window to finish
        ESP_ERROR_CHECK(rmt_wait_tx_done(task_inputs->rmt_channel, portMAX_DELAY));

        // read counter
        int16_t count = 0;
        ESP_ERROR_CHECK(pcnt_get_counter_value(task_inputs->pcnt_unit, &count));

        // TODO: check for overflow?

        frequency_hz = count / 2.0 / task_inputs->sampling_window_seconds;

        // call the frequency update callback
        if (task_inputs->frequency_update_callback)
        {
            (task_inputs->frequency_update_callback)(frequency_hz);
        }

        ESP_LOGD(TAG, "counter %d, frequency %f Hz", count, frequency_hz);

        int delay_time = task_inputs->sampling_period_seconds * 1000 / portTICK_PERIOD_MS;
        if (delay_time > 0)
        {
            vTaskDelayUntil(&last_wake_time, delay_time);
        }
    }

    free(rmt_items);
    free(task_inputs);  // TODO: avoid this if not dynamically allocated
    vTaskDelete(NULL);
}

