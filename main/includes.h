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




