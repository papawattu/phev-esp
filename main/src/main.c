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

#include "lwip/opt.h"

#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "apps/sntp/sntp.h"

#include "msg_mqtt.h"
#include "msg_gcp_mqtt.h"
#include "msg_pipe.h"
#include "msg_tcpip.h"

#include "phev_core.h"
#include "mqtt_client.h"

#include "jwt.h"

#include "json_bin.h"

extern const uint8_t rsa_private_pem_start[] asm("_binary_rsa_private_pem_start");
extern const uint8_t rsa_private_pem_end[]   asm("_binary_rsa_private_pem_end");

#define CONFIG_WIFI_SSID "BTHub3-HSZ3"
#define CONFIG_WIFI_PASSWORD "simpsons"

#define HOST_IP "192.168.8.46"
#define HOST_PORT 8080

//#define CONFIG_WIFI_SSID "REMOTE45cfsc"
//#define CONFIG_WIFI_PASSWORD "fhcm852767"

static EventGroupHandle_t wifi_event_group;

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
msg_pipe_ctx_t * connectPipe(void)
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
        .host = HOST_IP,
        .port = HOST_PORT,
        .connect = connectSocket, 
        .read = logRead,
        .write = logWrite,
    };
    
    messagingClient_t * in = msg_gcp_createGcpClient(inSettings);
    messagingClient_t * out = msg_tcpip_createTcpIpClient(outSettings);

    return msg_pipe(in, out);
}

message_t *addInput(message_t *message)
{
    message_t msg;
    msg.length = phev_core_encodeMessage(phev_core_simpleRequestCommandMessage(0x0a,0x01),&msg.data);
    return msg_core_copyMessage(&msg);
}
message_t *addOutput(message_t *message)
{
    char buf[255];
    phevMessage_t phevMsg;

    int remain = phev_core_firstMessage(message->data, &phevMsg);

//    while(remain > 0)
//    {
        uint8_t * buffer;
        phevMessage_t * msg = phev_core_responseHandler(&phevMsg);
        int len = phev_core_encodeMessage(msg, &buffer);
        lwip_write(global_sock,buffer,len);
//        remain = phev_core_firstMessage(message->data + remain, &phevMsg);
//    }
    snprintf(buf,255,"RESPONSE : Command %02X Length %d Type %d Register %d Data %02X Checksum %02X\n", phevMsg.command, phevMsg.length, phevMsg.type, phevMsg.reg, *phevMsg.data, phevMsg.checksum);
    snprintf(buf + strlen(buf),255,"RESPONSE : Command %02X Length %d Type %d Register %d Data %02X Checksum %02X\n", msg->command, msg->length, msg->type, msg->reg, *msg->data, msg->checksum);
    ESP_LOG_BUFFER_HEXDUMP(APP_TAG,buffer,len,ESP_LOG_INFO);
    message_t m = {
        .data = &buf,
        .length = strlen(buf),
    }; 
    return msg_core_copyMessage(&m);
}
void main_loop(void)
{
    msg_pipe_ctx_t *ctx = NULL;
    {
        ESP_LOGI(APP_TAG,"Waiting to connect...");
        ctx = connectPipe();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    } while(!(ctx->in->connected && ctx->out->connected));
    
    ESP_LOGI(APP_TAG,"TCPIP connected %d MQTT connected %d",ctx->out->connected,ctx->in->connected);

    msg_pipe_transformer_t transformer = {
        .input = addInput,
        .output = addOutput, //transformLightsJSONToBin,
    };

    msg_pipe_add_transformer(ctx, &transformer);

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
void start_app(void)
{
    ESP_LOGI(APP_TAG, "Application starting...");
    uint8_t new_mac[8] = {0x24, 0x0d, 0xc2, 0xc2, 0x91, 0x85};
    esp_base_mac_addr_set(new_mac);
    wifi_conn_init();
    // ppp_main();
    sntp_task();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
        
    main_loop();

}
void app_main(void)
{
    nvs_flash_init();
    tcpip_adapter_init();

    start_app();
}
