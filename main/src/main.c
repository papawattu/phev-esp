#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_types.h"
#include "nvs_flash.h"

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
#include "apps/sntp/sntp.h"

#include "msg_mqtt.h"
#include "msg_gcp_mqtt.h"
#include "msg_pipe.h"
#include "msg_tcpip.h"
#include "msg_utils.h"

#include "phev_core.h"
#include "phev_controller.h"
#include "mqtt_client.h"

#include "jwt.h"

#include "json_bin.h"

#include "ppp_client.h"

typedef struct {
    int type;  // the type of timer's event
    int timer_group;
    int timer_idx;
    uint64_t timer_counter_value;
} timer_event_t;

extern const uint8_t rsa_private_pem_start[] asm("_binary_rsa_private_pem_start");
extern const uint8_t rsa_private_pem_end[]   asm("_binary_rsa_private_pem_end");

static void wifi_conn_init(const char *ssid, const char * password);
void startTimer(void);

#define CONFIG_WIFI_SSID "BTHub6-P535"
#define CONFIG_WIFI_PASSWORD "S1mpsons"

//#define HOST_IP "35.205.234.94"
//#define HOST_PORT 8080

//#define HOST_IP "192.168.8.46"

//#define CONFIG_WIFI_SSID "REMOTE45cfsc"
//#define CONFIG_WIFI_PASSWORD "fhcm852767"

#define TIMER_DIVIDER         16  //  Hardware timer clock divider
#define TIMER_SCALE           (TIMER_BASE_CLK / TIMER_DIVIDER)  // convert counter value to seconds
#define INTERVAL_SEC 1.00

#define CONNECTED_CLIENTS "connectedClients"
#define CAR_CONNECTION "carConnection"
#define CAR_SSID "ssid"
#define CAR_PASSWORD "password"
#define CAR_HOST "host"
#define CAR_PORT "port"

static EventGroupHandle_t wifi_event_group;

xQueueHandle timer_queue;

const static int CONNECTED_BIT = BIT0;

const static char *APP_TAG = "Main";

msg_mqtt_t mqtt = {
    .init = &esp_mqtt_client_init,
    .start = &esp_mqtt_client_start,
    .publish = &esp_mqtt_client_publish,
    .subscribe = &esp_mqtt_client_subscribe
};

void getIatExp(char *iat, char *exp, int time_size)
{
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    
    while (timeinfo.tm_year < (2016 - 1900))
    {
        time(&now);
        localtime_r(&now, &timeinfo);
    }
    snprintf(iat, time_size, "%lu", now);
    snprintf(exp, time_size, "%lu", now + 3600);
} 
char *createJwt(const char *project_id)
{
    char iat_time[sizeof(time_t) * 3 + 2];
    char exp_time[sizeof(time_t) * 3 + 2];
    const uint8_t *key = (unsigned char *) rsa_private_pem_start;
    size_t key_len = rsa_private_pem_end - rsa_private_pem_start;
    jwt_t *jwt = NULL;
    int ret = 0; 
    char *out = NULL;

    getIatExp(iat_time, exp_time, sizeof(iat_time));
    
    jwt_new(&jwt);

    ret = jwt_add_grant(jwt, "iat", iat_time);
    if (ret)
    {
        printf("Error setting issue timestamp: %d", ret);
        return NULL;
    }
    ret = jwt_add_grant(jwt, "exp", exp_time);
    if (ret)
    {
        printf("Error setting expiration: %d", ret);
        return NULL;
    }
    ret = jwt_add_grant(jwt, "aud", project_id);
    if (ret)
    {
        printf("Error adding audience: %d", ret);
        return NULL;
    }
    ret = jwt_set_alg(jwt, JWT_ALG_RS256, key, key_len);
    if (ret)
    {
        printf("Error during set alg: %d", ret);
        return NULL;
    }
    out = jwt_encode_str(jwt);
    jwt_free(jwt);  
    return out;
}

int global_sock = 0;

int connectSocket(const char *host, uint16_t port) 
{
    struct sockaddr_in addr;
    /* set up address to connect to */
    memset(&addr, 0, sizeof(addr));
    addr.sin_len = sizeof(addr);
    addr.sin_family = AF_INET;
    addr.sin_port = PP_HTONS(port);
    addr.sin_addr.s_addr = inet_addr(host);

    ESP_LOGI(APP_TAG,"Host %s Port %d",host,port);
  /* create the sockets */
    int sock = lwip_socket(AF_INET, SOCK_STREAM, 0);
  //  LWIP_ASSERT("sock >= 0", sock >= 0);

    if (sock == -1)
    {
        return -1;
    }
    int ret = lwip_connect(sock, (struct sockaddr *)(&addr), sizeof(addr));
    if(ret == -1)
    {
        return -1;
    }
  //  LWIP_ASSERT("ret == 0", ret == 0);
    ESP_LOGI(APP_TAG,"Connected to host %s port %d",host,port);
    
    global_sock = sock;
    return sock;
}
int logRead(int soc, uint8_t * buf, size_t len)
{
    int num = lwip_read(soc,buf,len);
    ESP_LOGI(APP_TAG, "Read %d bytes",num);
    if(num > 0) 
    {
        ESP_LOG_BUFFER_HEXDUMP(APP_TAG,buf,num,ESP_LOG_INFO);
    }
    return num;
}
int logWrite(int soc, uint8_t * buf, size_t len)
{
    ESP_LOG_BUFFER_HEXDUMP(APP_TAG,buf,len,ESP_LOG_INFO);
    int num = lwip_write(soc,buf,len);
    ESP_LOGI(APP_TAG, "Written %d bytes",num);
    
    return num;
}

char * getConfigString(cJSON * json, char * option) 
{
    cJSON * value = cJSON_GetObjectItemCaseSensitive(json, option);
    if(value == NULL) {
        ESP_LOGE(APP_TAG,"Cannot find connection option %s", option);
        return NULL;
    }

    ESP_LOGI(APP_TAG,"Option %s set to %s", option, value->valuestring);
    
    return value->valuestring;
}
uint16_t getConfigInt(cJSON * json, char * option) 
{
    cJSON * value = cJSON_GetObjectItemCaseSensitive(json, option);
    if(value == NULL) {
        ESP_LOGE(APP_TAG,"Cannot find connection option %s", option);
        return NULL;
    }

    ESP_LOGI(APP_TAG,"Option %s set to %d", option, value->valueint);
    
    return value->valueint;
}
message_t * transformJSONToHex(void * ctx, message_t *message)
{
    cJSON *json = cJSON_Parse((const char *) message->data);

    cJSON *connect = NULL;

    connect = cJSON_GetObjectItemCaseSensitive(json, CONNECTED_CLIENTS);

    if(connect == NULL) 
    {
        return NULL;
    }
    if (connect->valueint > 0)
    {
        cJSON * carConnection = cJSON_GetObjectItemCaseSensitive(json, CAR_CONNECTION);
    
        phev_controller_setCarConnectionConfig(ctx, getConfigString(carConnection,CAR_SSID), 
                                                    getConfigString(carConnection,CAR_PASSWORD),
                                                    getConfigString(carConnection,CAR_HOST),
                                                    getConfigInt(carConnection,CAR_PORT)
                                                    );
        message_t out;
        uint8_t * data;
        uint8_t mac[] = {0xaa,0xbb,0xcc,0xdd,0xee,0x0a};
        
        out.length = phev_core_encodeMessage(phev_core_startMessage(2,mac),&data);
        out.data = data;
        return msg_utils_copyMsg(&out);
    }
    
    return NULL;
}
message_t * transformHexToJSON(void * ctx, phevMessage_t *message)
{
    char * output;

    cJSON * response = cJSON_CreateObject();
    if(response == NULL) 
    {
        ESP_LOGE(APP_TAG,"Cannot create JSON response");
        return NULL;
    }
    cJSON * command = cJSON_CreateNumber(message->command);
    if(command == NULL) 
    {
        ESP_LOGE(APP_TAG,"Cannot create JSON command response");
        return NULL;
    }
    cJSON_AddItemToObject(response, "command", command);
    
    cJSON * type = NULL;
    if(message->type == REQUEST_TYPE)
    {
        type = cJSON_CreateString("request");
    } else {
        type = cJSON_CreateString("response");
    }
    if(type == NULL) 
    {
        ESP_LOGE(APP_TAG,"Cannot create JSON type response");
        return NULL;
    }
    
    cJSON_AddItemToObject(response, "type", type);
    
    cJSON * length = cJSON_CreateNumber(message->length);
    if(length == NULL) 
    {
        ESP_LOGE(APP_TAG,"Cannot create JSON length response");
        return NULL;
    }
    cJSON_AddItemToObject(response, "length", length);  

    cJSON * data = cJSON_CreateArray();
    if(data == NULL) 
    {
        ESP_LOGE(APP_TAG,"Cannot create JSON data array response");
        return NULL;
    }
    cJSON_AddItemToObject(response, "data", data);  

    for(int i=0; i < message->length - 3; i++)
    {
        cJSON * item = cJSON_CreateNumber(message->data[i]);
        if (item == NULL)
        {
            return NULL;
        }
        cJSON_AddItemToArray(data, item);
    }

    output = cJSON_Print(response); 

    cJSON_Delete(response);

    return msg_utils_createMsg((uint8_t *) output, strlen(output));
}
int connectToCar(const char *host, uint16_t port)
{
    return connectSocket(host,port);
}
phevCtx_t * connectPipe(void)
{
    gcpSettings_t inSettings = {
        .host = "mqtt.googleapis.com",
        .port = 8883,
        .clientId = "projects/phev-db3fa/locations/us-central1/registries/my-registry/devices/my-device",
        .device = "my-device",
        .createJwt = createJwt,
        .mqtt = &mqtt,
        .projectId = "phev-db3fa",
        .topic = "/devices/my-device/events"
    }; 
    
    tcpIpSettings_t outSettings = {
        .connect = connectToCar, 
        .read = logRead,
        .write = logWrite,
    };
    
     
    phevSettings_t phev_settings = {
        .in = msg_gcp_createGcpClient(inSettings),
        .out = msg_tcpip_createTcpIpClient(outSettings),
        .inputTransformer = transformJSONToHex,
        .outputTransformer = transformHexToJSON,
        .startWifi = wifi_conn_init,
    };

    return phev_controller_init(&phev_settings);
}
/*
message_t *addInput(message_t *message)
{
    message_t msg;
    msg.length = phev_core_encodeMessage(phev_core_simpleRequestCommandMessage(0x0a,0x01),&msg.data);
    return msg_core_copyMessage(&msg);
}
message_t *addOutput(message_t *message)
{
    phevMessage_t phevMsg;

    int remain = phev_core_extractMessage(message->data, message->length, &phevMsg);

    return NULL; 
} */

void ping_task(void *pvParameter)
{
    phevCtx_t *ctx = (phevCtx_t *) pvParameter;

    ESP_LOGI(APP_TAG,"Ping task started");
    
    while(1)
    {
        while(ctx->pipe->in->connected && ctx->pipe->out->connected)
        {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            phev_controller_ping(ctx);
        }
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}
void main_loop(void)
{
    phevCtx_t *ctx = NULL;
    {
        ESP_LOGI(APP_TAG,"Waiting to connect...");
        ctx = connectPipe();
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    } while(!(ctx->pipe->in->connected && ctx->pipe->out->connected));
    
    ESP_LOGI(APP_TAG,"TCPIP connected %d MQTT connected %d",ctx->pipe->out->connected,ctx->pipe->in->connected);
/*
    msg_pipe_transformer_t transformer = {
        .input = addInput,
        .output = addOutput, //transformLightsJSONToBin,
    };

    msg_pipe_add_transformer(ctx, &transformer);
*/
    startTimer();
    
    xTaskCreate(&ping_task, "ping_task", 4096, (void *) ctx, 5, NULL);
    
    while(ctx->pipe->in->connected && ctx->pipe->out->connected)
    {
        msg_pipe_loop(ctx->pipe);
        
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
        ESP_LOGI(APP_TAG, "Wifi started");
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

static void wifi_conn_init(const char * wifiSSID, const char * wifiPassword)
{
    wifi_config_t wifi_config ={
        .sta.ssid = CONFIG_WIFI_SSID,
        .sta.password = CONFIG_WIFI_PASSWORD,
    };
    strncpy((char *) &wifi_config.sta.ssid, wifiSSID,strlen(wifiSSID));
    strncpy((char *) &wifi_config.sta.password, wifiPassword,strlen(wifiPassword));
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_LOGI(APP_TAG, "start the WIFI SSID:[%s] password:[%s]", wifiSSID, "******");
    ESP_ERROR_CHECK(esp_wifi_start());
    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
                        false, true, portMAX_DELAY);
}
static void sntp_task(void)
{
    int retry = 0;
    const int retry_count = 100;
    time_t now;
    struct tm timeinfo;

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
}
void wifiSetup(void)
{
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(wifi_event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
}
void start_app(void)
{
    ESP_LOGI(APP_TAG, "Application starting...");
    uint8_t new_mac[8] = {0x24, 0x0d, 0xc2, 0xc2, 0x91, 0x85};
    esp_base_mac_addr_set(new_mac);
    
    wifiSetup();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    
    ppp_main();
    sntp_task();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
        
    main_loop();

}
void IRAM_ATTR timer_group0_isr(void *para)
{
    int timer_idx = (int) para;

    //ESP_LOGI(APP_TAG,"Ping!!!");
    /* Retrieve the interrupt status and the counter value
       from the timer that reported the interrupt */
    uint32_t intr_status = TIMERG0.int_st_timers.val;
    TIMERG0.hw_timer[timer_idx].update = 1;
    uint64_t timer_counter_value = 
        ((uint64_t) TIMERG0.hw_timer[timer_idx].cnt_high) << 32
        | TIMERG0.hw_timer[timer_idx].cnt_low;

    /* Prepare basic event data
       that will be then sent back to the main program task */
    timer_event_t evt;
    evt.timer_group = 0;
    evt.timer_idx = timer_idx;
    evt.timer_counter_value = timer_counter_value;

    /* Clear the interrupt
       and update the alarm time for the timer with without reload */
    if ((intr_status & BIT(timer_idx)) && timer_idx == TIMER_0) {
        evt.type = 0;
        TIMERG0.int_clr_timers.t0 = 1;
        timer_counter_value += (uint64_t) (INTERVAL_SEC * TIMER_SCALE);
        TIMERG0.hw_timer[timer_idx].alarm_high = (uint32_t) (timer_counter_value >> 32);
        TIMERG0.hw_timer[timer_idx].alarm_low = (uint32_t) timer_counter_value;
    } else if ((intr_status & BIT(timer_idx)) && timer_idx == TIMER_1) {
        evt.type = 0;
        TIMERG0.int_clr_timers.t1 = 1;
    } else {
        evt.type = -1; // not supported even type
    }

    /* After the alarm has been triggered
      we need enable it again, so it is triggered the next time */
    TIMERG0.hw_timer[timer_idx].config.alarm_en = TIMER_ALARM_EN;

    /* Now just send the event data back to the main program task */
    xQueueSendFromISR(timer_queue, &evt, NULL);
}

void startTimer(void)
{
    timer_queue = xQueueCreate(10, sizeof(timer_event_t));
    
    timer_config_t config;
    config.divider = TIMER_DIVIDER;
    config.counter_dir = TIMER_COUNT_UP;
    config.counter_en = TIMER_PAUSE;
    config.alarm_en = TIMER_ALARM_EN;
    config.intr_type = TIMER_INTR_LEVEL;
    config.auto_reload = 0;
    timer_init(TIMER_GROUP_0, TIMER_0, &config);

    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0x00000000ULL);
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, INTERVAL_SEC * TIMER_SCALE);
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    timer_isr_register(TIMER_GROUP_0, TIMER_0, timer_group0_isr, 
        (void *) TIMER_0, ESP_INTR_FLAG_IRAM, NULL);

    timer_start(TIMER_GROUP_0, TIMER_0);

}
 
void app_main(void)
{
    nvs_flash_init();
    tcpip_adapter_init();
    //timer_example_evt_task(NULL);
    start_app();
}
