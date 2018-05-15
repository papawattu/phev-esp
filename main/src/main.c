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

#include "lwip/sockets.h"
#include "apps/sntp/sntp.h"

#include "msg_mqtt.h"
#include "msg_gcp_mqtt.h"
#include "msg_pipe.h"
#include "msg_tcpip.h"

#include "mqtt_client.h"

#include "jwt.h"

const char priv_key[] = {"-----BEGIN PRIVATE KEY-----\n\
MIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQDnoS3ICNP1y9c5\n\
DrY8StB2lAAyKh3tI+fYx85KHS3gixWRMF37GPzhN3FVwMdIGZrP7DWrmqLItIIT\n\
PP29dF3t/bhc1ByrzXlYCgdfzpZSwt839jkCENJznYWm2YfPW3Hq9ERm+mzZdOA5\n\
XuTVTx++in8kK9EjzOWwkeLiLdAcgVpeOrChP83sCx1gpQiIOMJh3pnSKMwZXTOx\n\
LGQQqtueip0OvY56Tm+6k0f8RmDpo65tPAVUFnVn0HbO0sbcZp7M7NeOWrKxjQ5w\n\
MSadQ8xd2jRAeUJ1OgFm9i27k+E2UjgwaG3b0eLtQ1RoJCOEgrfVrYIN1TeNhJkb\n\
pZXJy+kxAgMBAAECggEAWoLACf9xHAYoNrKKAWtWpE9W+EM+6HB0Y2BIOuvGBo0y\n\
w0Y8IydzgWoRzmmDNahTt1Xc7w6A956ZiXz2xfZX0eyoAxYbkgK7nOxmT0jBsttV\n\
MF/1jNoX8nemRzmPeTaJgD8tD3Rh3mNuTtzG3JeLlwA1iGPa2HtzcZjAKJOwU8J1\n\
wxJUlRkJkn+sMgdj7iVtVxiNVKOSidxRtyc6PLGmXuIsL2nNHnwAwfaroNtu7xKE\n\
UvgSygty9HHGHvYL6IXlCeBy8aReuCkzKoZTk516ALzmEaLAMKmfaanmmxflN9Ja\n\
aR2iIK6axN5f4XMboVbvmeUv/WE+BcguoCv2UR5yAQKBgQD2xRwNzssjqD2Q+rM/\n\
hrrHqihiEKJPF7eWv+bTbFUfcwcXH5vjAAYy4IhyFIPd1Kit8Bmazbq4cJi3VQMa\n\
mkxvon1A0g9y2HahkVK/sIjm6YLg4ptRFow+RO7Xi8IGHgdIqU7+Una4AlpTidR9\n\
uZymRgipwvWGYWV+saIvSfA/EQKBgQDwSxifIQ1I5Ms7eDtwj8IuNtuwu0S9WXes\n\
Zv2tJxcOchp/2bL+UNDG+8a0HzWvAu6sGoZu8S2soeujmSiVOwlnjufbuvilE7zf\n\
nO1Q19OucRAxTaT8D9u8PhNep2SH+hj9qwa61kBz2OUjI1th+NEgZcffrJjc6q61\n\
Z/VHzKdIIQKBgA5iNtxnLVgKumv08PXG+KQiuu/cYFmT3FOhI+c27U9D+SnT4dY9\n\
bh8RD2L25yjK+HoLALMRQsz1YzFsdx3Z2JK5OevJUrUzANcyDUy3EYPEIKjTDJKR\n\
aeNHSVSvBMQug+YsWDpivCDFolY0NnqNkx/t/RtIDd+jVbMoz3dKLALBAoGAW6Bn\n\
DRavWCJWFKEuGSfYJYMplmRyO7mH1qeXB5WmRw3faOz8QdlTIu0SwdA8yjVWCS48\n\
IMW6Yt+DWOin/u96EJD/fFv5QAkcti/CllcWEqhuQj6XML5jTY5mGGu/+9G0AdZb\n\
RJc6EHKePR3YVK1qIO5unq3TxDmA0Q+JCg3Ro8ECgYAeq4fk/bHvHV3zL93D8EqK\n\
igamI29JOVvLVHqmuQg2f1tsLn6vcg5tK1cRYp0ej4ArN1ND/f6xppCnMkb2zKlA\n\
R+ADc4eTc3/d0R7ahXDXg2qL76PDuN4+si8bCoxBnqtaGPKzJpe5OAqMvZ4s47CK\n\
jOAmKcVLejliuwOuflOncA==\n\
-----END PRIVATE KEY-----"};

extern const uint8_t rsa_private_pem_start[] asm("_binary_rsa_private_pem_start");
extern const uint8_t rsa_private_pem_end[]   asm("_binary_rsa_private_pem_end");

#define CONFIG_WIFI_SSID "BTHub3-HSZ3"
#define CONFIG_WIFI_PASSWORD "simpsons"

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

int connectSocket(const char *host, uint16_t port) 
{
    struct sockaddr_in remote_ip;
    bzero(&remote_ip, sizeof(struct sockaddr_in));
    remote_ip.sin_family = AF_INET;
    remote_ip.sin_port = htons(port);

    inet_aton(host, &(remote_ip.sin_addr));

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        return -1;
    }
    if (connect(sock, (struct sockaddr *)(&remote_ip), sizeof(struct sockaddr)) != 0)
    {
        return -1;
    }
    return sock;
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
    }; 
    
    tcpIpSettings_t outSettings = {
        .host = "192.168.8.46",
        .port = 8080,
        .connect = connectSocket, 
        .read = read,
        .write = write,
    };
    
    messagingClient_t * in = msg_gcp_createGcpClient(inSettings);
    messagingClient_t * out = msg_tcpip_createTcpIpClient(outSettings);

    return msg_pipe(in, out);
}

void main_loop(msg_pipe_ctx_t * ctx)
{
    
    while(!(ctx->in->connected && ctx->out->connected))
    {
        ESP_LOGI(APP_TAG,"Waiting to connect...");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
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

void connected(mqtt_event_handle_t *event)
{
    ESP_LOGI(APP_TAG, "MQTT Connected");

    subscribe((msg_mqtt_t *)((mqtt_event_t *)event)->user_context, "/topic/test");
}
void published(mqtt_event_handle_t *event)
{
    ESP_LOGI(APP_TAG, "MQTT published");
}
void incoming(msg_mqtt_t *mqtt, message_t *message)
{
    ESP_LOGI(APP_TAG, "MQTT Incoming message %s", message->data);
    printf("DATA=%.*s\r\n", message->length, message->data);
}
void subscribed(mqtt_event_handle_t *event)
{
    ESP_LOGI(APP_TAG, "MQTT subscribed");

    message_t message = {
        .data = (uint8_t *)"1234",
        .length = 4};
    message_t *msg = msg_core_copyMessage(&message);

    publish((msg_mqtt_t *)((mqtt_event_t *)event)->user_context, "/topic/test", msg);
}


void setupGCPClient(void)
{
    gcpSettings_t settings = {
        .host = "mqtt.googleapis.com",
        .port = 8883,
        .clientId = "projects/phev-db3fa/locations/us-central1/registries/my-registry/devices/my-device",
        .device = "my-device",
        .createJwt = createJwt,
        .mqtt = &mqtt,
        .projectId = "phev-db3fa",
    };

    messagingClient_t * client = msg_gcp_createGcpClient(settings);

    msg_gcp_connect(client);

}
void setupClient(void)
{
    msg_mqtt_settings_t settings = {
        .host = "iot.eclipse.org",
        .port = 0,
        .username = NULL,
        .password = NULL,
        .mqtt = &mqtt,
        .clientId = NULL,
        .username = NULL,
        .subscribed_cb = &subscribed,
        .connected_cb = &connected,
        .published_cb = &published,
        .incoming_cb = &incoming
    };
    handle_t handle = mqtt_start(&settings);
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
        
    msg_pipe_ctx_t *ctx = connectPipe();

    msg_pipe_transformer_t transformer = {
        .input = NULL,
        .output = NULL, //transformLightsJSONToBin
    };
    msg_pipe_add_transformer(ctx, &transformer);

    main_loop(ctx);

    //setupGCPClient();
}
void app_main(void)
{
    nvs_flash_init();
    tcpip_adapter_init();

    start_app();
}
