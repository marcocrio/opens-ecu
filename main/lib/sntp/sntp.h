#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"

//------------------SNTP-----------------------//

    /* Simple WiFi configuration that you can set via
    'make menuconfig'.
    If you'd rather not, just change the below entries to strings with
    the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
    */
    #define EXAMPLE_WIFI_SSID "Monkey-Pass" //network
    #define EXAMPLE_WIFI_PASS "free bananas" //password

    /* FreeRTOS event group to signal when we are connected & ready to make a request */
    static EventGroupHandle_t wifi_event_group;


    /* The event group allows multiple bits for each event,
    but we only care about one event - are we connected
    to the Access Point with an IP? */
    const int CONNECTED_BIT = BIT0;


    /* Variable holding number of times ESP32 restarted since first boot.
    * It is placed into RTC memory using RTC_DATA_ATTR and
    * maintains its value when ESP32 wakes from deep sleep.
    */
    RTC_DATA_ATTR static int boot_count = 0;

    time_t now;
    struct tm timeinfo;

    char strftime_buf[64];


    

void obtain_time(void);
void initialize_sntp(void);
void initialize_wifi(void);
esp_err_t event_handler(void *ctx, system_event_t *event);



#ifdef __cplusplus
}
#endif

