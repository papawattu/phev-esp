#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "car_connect.h"

const static char * APP_TAG = "Main";

void app_main(void)
{
    ESP_LOGI(APP_TAG,"Application starting...");
    nvs_flash_init();
    tcpip_adapter_init();
    
    car_connect();
}
