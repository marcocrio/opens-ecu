#include "includes.h"
#include "perinit.h"

#define PCNT_TEST_UNIT      PCNT_UNIT_0
#define PCNT_H_LIM_VAL      10
#define PCNT_L_LIM_VAL     -10
#define PCNT_THRESH1_VAL    5
#define PCNT_THRESH0_VAL   -5
#define PCNT_INPUT_SIG_IO   14  // Pulse Input GPIO
#define PCNT_INPUT_CTRL_IO  12  // Control GPIO HIGH=count up, LOW=count down
#define LEDC_OUTPUT_IO      18 // Output GPIO of a sample 1 Hz pulse generator

xQueueHandle pcnt_evt_queue;   // A queue to handle pulse counter events
pcnt_isr_handle_t user_isr_handle = NULL; //user's ISR service handle

// /* A sample structure to pass events from the PCNT
//  * interrupt handler to the main program.
//  */
typedef struct {
    int unit;  // the PCNT unit that originated an interrupt
    uint32_t status; // information on the event type that caused the interrupt
} pcnt_evt_t;

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
        frequency = 1;
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
        vTaskDelay(1);
    }
};

//*****************************************************************************************//
//*****************************************************************************************//
//************************************* Frequency Counter ********************************************//
//*****************************************************************************************//
//*****************************************************************************************//

void IRAM_ATTR pcnt_example_intr_handler(void *arg)
{
    uint32_t intr_status = PCNT.int_st.val;
    int i;
    pcnt_evt_t evt;
    portBASE_TYPE HPTaskAwoken = pdFALSE;

    for (i = 0; i < PCNT_UNIT_MAX; i++) {
        if (intr_status & (BIT(i))) {
            evt.unit = i;
            /* Save the PCNT event type that caused an interrupt
               to pass it to the main program */
            evt.status = PCNT.status_unit[i].val;
            PCNT.int_clr.val = BIT(i);
            xQueueSendFromISR(pcnt_evt_queue, &evt, &HPTaskAwoken);
            if (HPTaskAwoken == pdTRUE) {
                portYIELD_FROM_ISR();
            }
        }
    }
}

void pcnt_example_init(void)
{
    /* Prepare configuration for the PCNT unit */
    pcnt_config_t pcnt_config = {
        // Set PCNT input signal and control GPIOs
        .pulse_gpio_num = PCNT_INPUT_SIG_IO,
        .ctrl_gpio_num = PCNT_INPUT_CTRL_IO,
        .channel = PCNT_CHANNEL_0,
        .unit = PCNT_TEST_UNIT,
        // What to do on the positive / negative edge of pulse input?
        .pos_mode = PCNT_COUNT_INC,   // Count up on the positive edge
        .neg_mode = PCNT_COUNT_DIS,   // Keep the counter value on the negative edge
        // What to do when control input is low or high?
        .lctrl_mode = PCNT_MODE_REVERSE, // Reverse counting direction if low
        .hctrl_mode = PCNT_MODE_KEEP,    // Keep the primary counter mode if high
        // Set the maximum and minimum limit values to watch
        .counter_h_lim = PCNT_H_LIM_VAL,
        .counter_l_lim = PCNT_L_LIM_VAL,
    };
    /* Initialize PCNT unit */
    pcnt_unit_config(&pcnt_config);

    /* Configure and enable the input filter */
    pcnt_set_filter_value(PCNT_TEST_UNIT, 100);
    pcnt_filter_enable(PCNT_TEST_UNIT);

    /* Set threshold 0 and 1 values and enable events to watch */
    pcnt_set_event_value(PCNT_TEST_UNIT, PCNT_EVT_THRES_1, PCNT_THRESH1_VAL);
    pcnt_event_enable(PCNT_TEST_UNIT, PCNT_EVT_THRES_1);
    pcnt_set_event_value(PCNT_TEST_UNIT, PCNT_EVT_THRES_0, PCNT_THRESH0_VAL);
    pcnt_event_enable(PCNT_TEST_UNIT, PCNT_EVT_THRES_0);
    /* Enable events on zero, maximum and minimum limit values */
    pcnt_event_enable(PCNT_TEST_UNIT, PCNT_EVT_ZERO);
    pcnt_event_enable(PCNT_TEST_UNIT, PCNT_EVT_H_LIM);
    pcnt_event_enable(PCNT_TEST_UNIT, PCNT_EVT_L_LIM);

    /* Initialize PCNT's counter */
    pcnt_counter_pause(PCNT_TEST_UNIT);
    pcnt_counter_clear(PCNT_TEST_UNIT);

    /* Register ISR handler and enable interrupts for PCNT unit */
    pcnt_isr_register(pcnt_example_intr_handler, NULL, 0, &user_isr_handle);
    pcnt_intr_enable(PCNT_TEST_UNIT);

    /* Everything is set up, now go to counting */
    pcnt_counter_resume(PCNT_TEST_UNIT);
}

void PulseCounter(void *pvParameter)
{

    /* Initialize PCNT event queue and PCNT functions */
    pcnt_evt_queue = xQueueCreate(10, sizeof(pcnt_evt_t));
    pcnt_example_init();

    int16_t count = 0;
    pcnt_evt_t evt;
    portBASE_TYPE res;
    while (1) {
        /* Wait for the event information passed from PCNT's interrupt handler.
         * Once received, decode the event type and print it on the serial monitor.
         */
        res = xQueueReceive(pcnt_evt_queue, &evt, 1000 / portTICK_PERIOD_MS);
        if (res == pdTRUE) {
            pcnt_get_counter_value(PCNT_TEST_UNIT, &count);
            printf("Event PCNT unit[%d]; cnt: %d\n", evt.unit, count);
            if (evt.status & PCNT_EVT_THRES_1) {
                printf("THRES1 EVT\n");
            }
            if (evt.status & PCNT_EVT_THRES_0) {
                printf("THRES0 EVT\n");
            }
            if (evt.status & PCNT_EVT_L_LIM) {
                printf("L_LIM EVT\n");
            }
            if (evt.status & PCNT_EVT_H_LIM) {
                printf("H_LIM EVT\n");
            }
            if (evt.status & PCNT_EVT_ZERO) {
                printf("ZERO EVT\n");
            }
        } else {
            pcnt_get_counter_value(PCNT_TEST_UNIT, &count);
            printf("\n-----------------\nCurrent counter value :%d\n-----------------\n", count);
            vTaskDelay(500/portTICK_PERIOD_MS);
        }
    }
    if(user_isr_handle) {
        //Free the ISR service handle.
        esp_intr_free(user_isr_handle);
        user_isr_handle = NULL;
    }

    vTaskDelay(500/portTICK_PERIOD_MS);
    //vTaskDelay(2);
    
}
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

