#ifdef __XTENSA__
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_types.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "soc/timer_group_struct.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"

#include "lwip/opt.h"

#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "lwip/ip.h"
#include "lwip/apps/sntp.h"
//#include "apps/sntp/sntp.h"
#include "lwip/netif.h"

//#include "esp_ota_ops.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "app.h"
#include "msg_mqtt.h"
#include "msg_gcp_mqtt.h"
#include "msg_pipe.h"
#include "msg_tcpip.h"
#include "msg_utils.h"

#include "phev_core.h"
#include "phev_controller.h"
#include "mqtt_client.h"
#include "wifi_client.h"
#include <esp_http_server.h>
#include "phev_setup.h"
#include "mdns.h"

#include "jwt.h"

//#include "ppp_client.h"

//#include "captdns.h"

#define WEB_SERVER "www.bbc.co.uk"
#define WEB_PORT 80
#define WEB_URL "http://www.bbc.co.uk/"

static const char *TAG = "example";

static const char *REQUEST = "GET " WEB_URL " HTTP/1.0\r\n"
    "Host: "WEB_SERVER"\r\n"
    "User-Agent: esp-idf/1.0 esp32\r\n"
    "\r\n";
#define MQTT_MAX_CLIENT_LEN         128
#define MQTT_MAX_USERNAME_LEN       128
#define MQTT_MAX_PASSWORD_LEN       128
#define MQTT_MAX_HOST_LEN           128

#define NO_PPP
#define NO_OTA

#define GSM_RESET_PIN GPIO_NUM_21
#define UPDATE_NAME "update"
#define UPDATE_SSID "ssid"
#define UPDATE_PASSWORD "password"
#define UPDATE_HOST "host"
#define UPDATE_PATH "path"
#define UPDATE_PORT "port"
#define LATEST_BUILD "latestBuild"
#define UPDATE_OVER_GSM "overGsm"
#define FORCE_UPDATE "forceUpdate"

#define EXAMPLE_MDNS_HOSTNAME "phevremote"
#define EXAMPLE_MDNS_INSTANCE "instance"

#ifndef BUILD_NUMBER
#define BUILD_NUMBER 1
#endif
typedef struct {
    int type;  // the type of timer's event
    int timer_group;
    int timer_idx;
    uint64_t timer_counter_value;
} timer_event_t;

static bool connected = false;
//esp_mqtt_client_handle_t client;


#define CONFIG_WIFI_SSID "REMOTE45cfsc"
#define CONFIG_WIFI_PASSWORD "fhcm852767"

#define TIMER_DIVIDER         16  //  Hardware timer clock divider
#define TIMER_SCALE           (TIMER_BASE_CLK / TIMER_DIVIDER)  // convert counter value to seconds
#define INTERVAL_SEC 1.00

#define CONNECTED_CLIENTS "connectedClients"
#define CAR_CONNECTION "carConnection"
#define CAR_SSID "ssid"
#define CAR_PASSWORD "password"
#define CAR_HOST "host"
#define CAR_PORT "port"

#define EXAMPLE_FILENAME "test"
#define EXAMPLE_SERVER_IP "192.168.1.1"
#define EXAMPLE_SERVER_PORT 8080

#define BUFFSIZE 1024
#define TEXT_BUFFSIZE 1024

static char ota_write_data[BUFFSIZE + 1] = { 0 };
/*an packet receive buffer*/
static char text[BUFFSIZE + 1] = { 0 };

static int binary_file_length = 0;

static EventGroupHandle_t mqtt_event_group;

xQueueHandle timer_queue;

const static char *APP_TAG = "Main";

static void http_get_task(void *pvParameters);

extern const char config_json_start[] asm("_binary_config_json_start");
extern const char config_json_end[]   asm("_binary_config_json_end");

void messagePublished(mqtt_event_handle_t event)
{
    ESP_LOGD(APP_TAG,"Publish Callback");
}

msg_mqtt_t mqtt = {
    .init = esp_mqtt_client_init,
    .start = esp_mqtt_client_start,
    .publish = esp_mqtt_client_publish,
    .subscribe = esp_mqtt_client_subscribe
};
void main_loop(void *pvParameter)
{
    ESP_LOGI(APP_TAG,"Main Loop");
    phevCtx_t * ctx = app_createPhevController(mqtt);

//    phev_controller_setConfigJson(ctx, config_json_start);
    
//    ESP_LOGI(APP_TAG,"Config set...");

//    char * configTxt = app_displayConfig(ctx->config); 
    
//    ESP_LOGI(APP_TAG,"Build version :%d", ctx->config->updateConfig.currentBuild);
//    ESP_LOGI(APP_TAG,"Latest build version :%d", ctx->config->updateConfig.latestBuild);
//    ESP_LOGI(APP_TAG,"%s",configTxt);

//    phev_controller_updateConfig(ctx, ctx->config);
    
    ESP_LOGI(APP_TAG,"Waiting to connect...");
    
    while(!ctx->pipe->in->connected)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(APP_TAG,"Connected To GCP IOT...");
    
    int x = 0;
    while(1) 
    {
        if(!ctx->otaUpdating) 
        {
            phev_controller_eventLoop(ctx);
            vTaskDelay(50 / portTICK_PERIOD_MS);
            x++;
            if(x % 100 == 0) 
            {
                x = 0;
                ESP_LOGI(APP_TAG, "Free heap %d", system_get_free_heap_size());
            }
        }  
    } 
}

static void sntp_task(void)
{
    int retry = 0;
    const int retry_count = 100;
    time_t now;
    struct tm timeinfo;
    char strftime_buf[64];

    ESP_LOGI(APP_TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();

    while (timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count)
    {
        ESP_LOGI(APP_TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }
    ESP_LOGI(APP_TAG, "Time synced");
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI(APP_TAG, "The current date/time is: %s", strftime_buf);
}

void start_app(void)
{
    static httpd_handle_t server = NULL;

    ESP_LOGI(APP_TAG, "Application starting...");
    
    wifi_client_setup();

    vTaskDelay(2000 / portTICK_PERIOD_MS);
    wifi_ap_init(&server);    
    
    connectionDetails_t * details = setup_ui_getConnectionDetails();


#ifndef NO_PPP    
    ESP_LOGD(APP_TAG, "PPP starting...");
    
    pppConnectionDetails_t connectionDetails = {
        .user = details->pppUser,
        .password = details->pppPassword,
        .apn = details->pppAPN,
    };
    ppp_main(&connectionDetails);
    
    for (struct netif *pri = netif_list; pri != NULL; pri=pri->next)
    {
        ESP_LOGD(APP_TAG, "Interface priority is %c%c%d (" IPSTR "/" IPSTR " gateway " IPSTR ")",
        pri->name[0], pri->name[1], pri->num,
        IP2STR(&pri->ip_addr.u_addr.ip4), IP2STR(&pri->netmask.u_addr.ip4), IP2STR(&pri->gw.u_addr.ip4));
        if(pri->name[0] == 'p') netif_set_default(pri);
    } 
#else
    //wifi_conn_init("BTHub6-P535", "S1mpsons",false);
    wifi_ap_init(&server);    
    
    httpd_handle_t * httpServer = start_webserver();
#endif
    //ESP_LOGD(APP_TAG, "PPP delay...");
    //xTaskCreate(&http_get_task, "http_get_task", 4096, NULL, 5, NULL);
    //vTaskDelay(5000 / portTICK_PERIOD_MS);
    ESP_LOGD(APP_TAG, "SNTP starting...");
    sntp_task();
    //vTaskDelay(2000 / portTICK_PERIOD_MS);
    ESP_LOGD(APP_TAG, "Main starting...");

    xTaskCreate(&main_loop, "main_task", 4096, NULL, 5, NULL);    
    
}
void resetGSMModule(int resetPin)
{
    ESP_LOGI(APP_TAG,"Resetting GSM module");
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = 1ULL << resetPin;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
    gpio_set_level(resetPin, 0);
    //ESP_LOGI(APP_TAG,"Before Level %d", gpio_get_level(GPIO_NUM_19));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    gpio_set_level(resetPin, 1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    //ESP_LOGI(APP_TAG,"After Level %d", gpio_get_level(GPIO_NUM_19));
    gpio_set_level(resetPin, 0);
}

static void initialise_mdns(void)
{
    //initialize mDNS
    ESP_ERROR_CHECK( mdns_init() );
    //set mDNS hostname (required if you want to advertise services)
    ESP_ERROR_CHECK( mdns_hostname_set(EXAMPLE_MDNS_HOSTNAME) );
    //set default mDNS instance name
    ESP_ERROR_CHECK( mdns_instance_name_set(EXAMPLE_MDNS_INSTANCE) );

    //structure with TXT records
    mdns_txt_item_t serviceTxtData[3] = {
        {"board","esp32"},
        {"u","user"},
        {"p","password"}
    };

    //initialize service
    ESP_ERROR_CHECK( mdns_service_add("ESP32-WebServer", "_http", "_tcp", 80, serviceTxtData, 3) );
    //add another TXT item
    ESP_ERROR_CHECK( mdns_service_txt_item_set("_http", "_tcp", "path", "/foobar") );
    //change TXT item value
    ESP_ERROR_CHECK( mdns_service_txt_item_set("_http", "_tcp", "u", "admin") );
}

static void http_get_task(void *pvParameters)
{
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    struct in_addr *addr;
    int s, r;
    char recv_buf[64];

    while(1) {
        /* Wait for the callback to set the CONNECTED_BIT in the
           event group.
        */
       // xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
       //                     false, true, portMAX_DELAY);
      //  ESP_LOGI(APP_TAG, "Connected to AP");

        int err = getaddrinfo(WEB_SERVER, "80", &hints, &res);

        if(err != 0 || res == NULL) {
            ESP_LOGE(APP_TAG, "DNS lookup failed err=%d res=%p", err, res);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        /* Code to print the resolved IP.

           Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
        addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
        ESP_LOGI(APP_TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

        s = socket(res->ai_family, res->ai_socktype, 0);
        if(s < 0) {
            ESP_LOGE(APP_TAG, "... Failed to allocate socket.");
            freeaddrinfo(res);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(APP_TAG, "... allocated socket");

        if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
            ESP_LOGE(APP_TAG, "... socket connect failed errno=%d", errno);
            close(s);
            freeaddrinfo(res);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }

        ESP_LOGI(APP_TAG, "... connected");
        freeaddrinfo(res);

        if (write(s, REQUEST, strlen(REQUEST)) < 0) {
            ESP_LOGE(APP_TAG, "... socket send failed");
            close(s);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(APP_TAG, "... socket send success");

        struct timeval receiving_timeout;
        receiving_timeout.tv_sec = 5;
        receiving_timeout.tv_usec = 0;
        if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
                sizeof(receiving_timeout)) < 0) {
            ESP_LOGE(APP_TAG, "... failed to set socket receiving timeout");
            close(s);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(APP_TAG, "... set socket receiving timeout success");

        /* Read HTTP response */
        do {
            bzero(recv_buf, sizeof(recv_buf));
            r = read(s, recv_buf, sizeof(recv_buf)-1);
            for(int i = 0; i < r; i++) {
                putchar(recv_buf[i]);
            }
        } while(r > 0);

        ESP_LOGI(APP_TAG, "... done reading from socket. Last read return=%d errno=%d\r\n", r, errno);
        close(s);
        for(int countdown = 10; countdown >= 0; countdown--) {
            ESP_LOGI(APP_TAG, "%d... ", countdown);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        ESP_LOGI(APP_TAG, "Starting again!");
    }
}
void app_main(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        // OTA app partition table has a smaller NVS partition size than the non-OTA
        // partition table. This size mismatch may cause NVS initialization to fail.
        // If this happens, we erase NVS partition and initialize NVS again.
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
    resetGSMModule(GPIO_NUM_21);
    tcpip_adapter_init();

    //timer_example_evt_task(NULL);
    
    ESP_LOGI(APP_TAG,"PHEV ESP Build %lld", (long long int) BUILD_NUMBER);
    //initialise_mdns();
    //captdnsInit();
    start_app();
}

#endif // __XTENSA__