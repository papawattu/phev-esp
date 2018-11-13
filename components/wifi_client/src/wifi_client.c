#include "wifi_client.h"

#ifdef __linux__
void wifi_conn_init(const char * wifiSSID, const char * wifiPassword, const bool setPPPdefault)
{

}
#else
#include "esp_log.h"
#include "lwip/netif.h"
#include "esp_wifi.h"
#include "freertos/event_groups.h"
#include <string.h>
#include "esp_event.h"
#include "esp_event_loop.h"
#include "setup_ui.h"
#include "mdns.h"

#define httpd_handle_t void
const static char *APP_TAG = "WIFI_CLIENT";
const static int CONNECTED_BIT = BIT0;
static EventGroupHandle_t wifi_event_group;

static esp_err_t wifi_client_event_handler(void *ctx, system_event_t *event)
{
    httpd_handle_t *server = (httpd_handle_t *) ctx;

    switch (event->event_id)
    {
    case SYSTEM_EVENT_AP_STACONNECTED: 
    {
        ESP_LOGI(APP_TAG, "station:"MACSTR" join, AID=%d",
            MAC2STR(event->event_info.sta_connected.mac),
        event->event_info.sta_connected.aid);
        //tcpip_adapter_create_ip6_linklocal(TCPIP_ADAPTER_IF_AP);
        if (server == NULL) {
            ///server = start_webserver();
        }
        break;
    }
    case SYSTEM_EVENT_AP_STADISCONNECTED:
        ESP_LOGI(APP_TAG, "station:"MACSTR"leave, AID=%d",
                 MAC2STR(event->event_info.sta_disconnected.mac),
                 event->event_info.sta_disconnected.aid);
        break;
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        //tcpip_adapter_create_ip6_linklocal(TCPIP_ADAPTER_IF_STA);
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        ESP_LOGI(APP_TAG, "Wifi started");
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        /* This is a workaround as ESP32 WiFi libs don't currently
               auto-reassociate. */
        //esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        esp_restart();
        break;
    default:
        break;
    }
    //mdns_handle_system_event(ctx, event);
    return ESP_OK;
}

void wifi_client_setup(void)
{
    uint8_t new_mac[8] = {0x24, 0x0d, 0xc2, 0xc2, 0x91, 0x85};
    esp_base_mac_addr_set(new_mac);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(wifi_client_event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
}

void wifi_ap_init(void * arg)
{
    //ESP_ERROR_CHECK(esp_event_loop_init(event_handler, arg));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_ap_config =  {
        .ap = {
            .ssid = AP_WIFI_SSID,
            .ssid_len = strlen(AP_WIFI_SSID),
            .password = AP_WIFI_PASS,
            .max_connection = AP_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },            
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_ap_config));

}
void wifi_conn_init(const char * wifiSSID, const char * wifiPassword, const bool setPPPdefault)
{
    esp_wifi_stop();
    wifi_config_t wifi_config = {
        .sta.ssid = "",
        .sta.password = "",
    };
    
    strncpy((char *) wifi_config.sta.ssid, wifiSSID,MAX_WIFI_CLIENT_SSID_LEN);
    strncpy((char *) wifi_config.sta.password, wifiPassword,MAX_WIFI_CLIENT_PASSWORD_LEN);
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_LOGI(APP_TAG, "start the WIFI SSID:[%s] password:[%s]", wifiSSID, wifiPassword);
    ESP_ERROR_CHECK(esp_wifi_start());
    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
                        false, true, portMAX_DELAY);
    for (struct netif *pri = netif_list; pri != NULL; pri=pri->next)
    {
        ESP_LOGD(APP_TAG, "Interface priority is %c%c%d (" IPSTR "/" IPSTR " gateway " IPSTR ")",
        pri->name[0], pri->name[1], pri->num,
        IP2STR(&pri->ip_addr.u_addr.ip4), IP2STR(&pri->netmask.u_addr.ip4), IP2STR(&pri->gw.u_addr.ip4));
        if(pri->name[0] == 'p' && setPPPdefault) netif_set_default(pri);
    }
}

#endif