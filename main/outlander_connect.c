#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "msg_pipe.h"
#include "msg_tcpip.h"
#include "msg_gcp_mqtt.h"

#define CONFIG_WIFI_SSID "BTHub3-HSZ3"
#define CONFIG_WIFI_PASSWORD "simpsons"

static EventGroupHandle_t wifi_event_group;

const static int CONNECTED_BIT = BIT0;

const static char * APP_TAG = "Main";

msg_pipe_ctx_t * connect(void)
{
    tcpIpSettings_t settings;
    settings.host = "192.168.1.103";
    settings.port = 8082;
    messagingClient_t * in = msg_tcpip_createTcpIpClient(settings);
    ESP_LOGI(APP_TAG,"Incoming Host %s",((tcpip_ctx_t *) in->ctx)->host);
    ESP_LOGI(APP_TAG,"Incoming Port %d",((tcpip_ctx_t *) in->ctx)->port);
    
    settings.host = "192.168.1.103";
    settings.port = 8081;
    messagingClient_t * out = msg_tcpip_createTcpIpClient(settings);
    
    ESP_LOGI(APP_TAG,"Outgoing Host %s",((tcpip_ctx_t *) out->ctx)->host);
    ESP_LOGI(APP_TAG,"Outgoing Port %d",((tcpip_ctx_t *) out->ctx)->port);
    
    return msg_pipe(in, out);
}

void main_loop(msg_pipe_ctx_t * ctx)
{
    while(ctx->in->connected && ctx->out->connected)
    {
        msg_pipe_loop(ctx);
        vTaskDelay(portTICK_PERIOD_MS);
    }

    ESP_LOGI(APP_TAG,"Disconnected...");
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    ESP_LOGI(APP_TAG,"Restarting...");
    esp_restart();
}
static esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id)
    {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        ESP_LOGI(APP_TAG, "Wifi started")
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        /* This is a workaround as ESP32 WiFi libs don't currently
               auto-reassociate. */
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

static void wifi_conn_init(void)
{
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(wifi_event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_WIFI_SSID,
            .password = CONFIG_WIFI_PASSWORD,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_LOGI(APP_TAG, "start the WIFI SSID:[%s] password:[%s]", CONFIG_WIFI_SSID, "******");
    ESP_ERROR_CHECK(esp_wifi_start());
}
void start_app(void)
{
    ESP_LOGI(APP_TAG,"Application starting...");
    nvs_flash_init();
    tcpip_adapter_init();
    wifi_conn_init();
    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
                       false, true, portMAX_DELAY);
    msg_pipe_ctx_t *ctx = connect();

    main_loop(ctx);

}
void app_main(void)
{
    start_app();
}
