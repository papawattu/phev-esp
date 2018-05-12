
#include "msg_mqtt.h"
//#include "esp_log.h"
#include "mqtt_client.h"

static const char *TAG = "MQTT_SAMPLE";

//static EventGroupHandle_t wifi_event_group;
//const static int CONNECTED_BIT = BIT0;


static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    msg_mqtt_ctx_t *ctx = event->user_context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
//            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            msg_id = ctx->esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
//            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = ctx->esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
//            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = ctx->esp_mqtt_client_unsubscribe(client, "/topic/qos1");
//            ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED:
 //           ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
  //          ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            msg_id = ctx->esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
  //          ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
 //           ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
//            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
//            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
//            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
    }
    return ESP_OK;
}

static void mqtt_app_start(msg_mqtt_ctx_t * ctx)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtt://iot.eclipse.org",
        .event_handle = mqtt_event_handler,
        .user_context = (void *) ctx
    };

    esp_mqtt_client_handle_t client = ctx->esp_mqtt_client_init(&mqtt_cfg);
    ctx->esp_mqtt_client_start(client);
}