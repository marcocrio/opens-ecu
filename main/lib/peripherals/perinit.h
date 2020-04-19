#pragma once
#ifndef FREQUENCY_COUNT_H
#endif
#define FREQUENCY_COUNT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../includes.h"

// Register WiFi functions
void setADC(); //Set up ADC at 10 bit
void set_injPWM(float freq, float duty); //Sets up PWM
void deb();
void pwm_signals(void *pvParameter);
void perinit_display(void *pvParameter);
void IRAM_ATTR pcnt_example_intr_handler(void *arg);
void pcnt_example_init(void);
void PulseCounter(void *pvParameter);

// typedef struct
// {
//     gpio_num_t pcnt_gpio;                   ///< count events on this GPIO
//     pcnt_unit_t pcnt_unit;                  ///< PCNT unit to use for counting
//     pcnt_channel_t pcnt_channel;            ///< PCNT channel to use for counting
//     gpio_num_t rmt_gpio;                    ///< used by RMT to define a sampling window
//     rmt_channel_t rmt_channel;              ///< The RMT channel to use
//     uint8_t rmt_clk_div;                    ///< RMT pulse length, as a divider of the APB clock
//     uint8_t rmt_max_blocks;                 ///< Maximum number of RMT RAM blocks that can be used by task. Each block provides 64 RMT items and each item can provide a window duration of up to 2 * 32767 = 65534 periods.
//     float sampling_period_seconds;          ///< time (in seconds) between start of adjacent samples
//     float sampling_window_seconds;          ///< sample window length (in seconds)
//     uint16_t filter_length;                 ///< counter filter length in APB cycles
//     void (*window_start_callback)(void);    ///< called just prior to starting a sampling window
//     void (*frequency_update_callback)(double hz);  ///< called each time a frequency is determined
// } frequency_count_configuration_t;

// /**
//  * @brief Task function - pass this to xTaskCreate with a pointer to an instance of frequency_count_configuration_t as the parameter.
//  */
// void frequency_count_task_function(void * pvParameter);

#ifdef __cplusplus
}
#endif

