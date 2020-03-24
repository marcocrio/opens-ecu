

// //standar freeRTOS
// #include <stdlib.h>
// #include <stdio.h>
// #include <string.h>
// #include <math.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "esp_spi_flash.h"
// #include "driver/gpio.h"
// #include "sdkconfig.h"
// #include "freertos/queue.h"



// #include <stdio.h>
// #include <string.h>
// #include "esp_system.h"
// #include "esp_log.h"
// #include "esp_console.h"
// #include "esp_vfs_dev.h"
// #include "driver/uart.h"
// #include "linenoise/linenoise.h"
// #include "argtable3/argtable3.h"
// #include "cmd_decl.h"
// #include "esp_vfs_fat.h"
// #include "nvs.h"
// #include "nvs_flash.h"




// //sntp (get time over the air)
// #include <time.h>
// #include <sys/time.h>
// #include "freertos/event_groups.h"
// #include "esp_system.h"
// #include "esp_wifi.h"
// #include "esp_event_loop.h"
// #include "esp_attr.h"
// #include "esp_sleep.h"
// #include "nvs_flash.h"

// #include "lwip/err.h"
// #include "lwip/apps/sntp.h"





// //*****************************************************************************************//
// //*****************************************************************************************//
// //*************************************** SNTP ********************************************//
// //*****************************************************************************************//
// //*****************************************************************************************//



//     void obtain_time(void)
//     {
//         ESP_ERROR_CHECK( nvs_flash_init() );
//         initialize_wifi();
//         xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
//                             false, true, portMAX_DELAY);
//         initialize_sntp();

//         // wait for time to be set
//         time_t now = 0;
//         struct tm timeinfo = { 0 };
//         int retry = 0;
//         const int retry_count = 10;
//         while(timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count) {
//             ESP_LOGI(SNTP, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
//             vTaskDelay(2000 / portTICK_PERIOD_MS);
//             time(&now);
//             localtime_r(&now, &timeinfo);
//         }

//         ESP_ERROR_CHECK( esp_wifi_stop() );
//     }

//     void initialize_sntp(void)
//     {
//         ESP_LOGI(SNTP, "Initializing SNTP");
//         sntp_setoperatingmode(SNTP_OPMODE_POLL);
//         sntp_setservername(0, "pool.ntp.org");
//         sntp_init();
//     }

//     void initialize_wifi(void)
//     {
//         tcpip_adapter_init();
//         wifi_event_group = xEventGroupCreate();
//         ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
//         wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//         ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
//         ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
//         wifi_config_t wifi_config = {
//             .sta = {
//                 .ssid = EXAMPLE_WIFI_SSID,
//                 .password = EXAMPLE_WIFI_PASS,
//             },
//         };
//         ESP_LOGI(SNTP, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
//         ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
//         ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
//         ESP_ERROR_CHECK( esp_wifi_start() );
//     }

//     esp_err_t event_handler(void *ctx, system_event_t *event)
//     {
//         switch(event->event_id) {
//         case SYSTEM_EVENT_STA_START:
//             esp_wifi_connect();
//             break;
//         case SYSTEM_EVENT_STA_GOT_IP:
//             xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
//             break;
//         case SYSTEM_EVENT_STA_DISCONNECTED:
//             /* This is a workaround as ESP32 WiFi libs don't currently
//             auto-reassociate. */
//             esp_wifi_connect();
//             xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
//             break;
//         default:
//             break;
//         }
//         return ESP_OK;
//     }