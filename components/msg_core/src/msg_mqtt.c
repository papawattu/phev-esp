

#include "msg_core.h"
#include "msg_mqtt.h"
//#include "mqtt_client.h"

//static const char *TAG = "MQTT_SAMPLE";

//static EventGroupHandle_t wifi_event_group;
//const static int CONNECTED_BIT = BIT0;

void eventData(mqtt_event_handle_t event)
{
    char *topic = malloc(event->topic_len + 1);
    memcpy(topic, event->topic, event->topic_len);
    topic[event->topic_len] = 0;
    free(topic);
    

    uint8_t *data = malloc(event->data_len + 1);
    memcpy(data, event->data, event->data_len);
    data[event->data_len] = 0;
    
    message_t message = {
        .data = data,
        .length = event->data_len
    };

    msg_core_call_subs(((msg_mqtt_t *) event->user_context)->client, &message);

}

static err_t mqtt_event_handler(mqtt_event_handle_t event)
{
    handle_t client = event->client;
    int msg_id;
    msg_mqtt_t *ctx = event->user_context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            msg_id = ctx->subscribe(client, "/topic/qos0", 0);
            msg_id = ctx->subscribe(client, "/topic/qos1", 1);
            msg_id = ctx->unsubscribe(client, "/topic/qos1");
            break;
        case MQTT_EVENT_DISCONNECTED:
            break;
        case MQTT_EVENT_SUBSCRIBED:
            msg_id = ctx->publish(client, "/topic/qos0", "data", 0, 0, 0);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            break;
        case MQTT_EVENT_PUBLISHED:
            break;
        case MQTT_EVENT_DATA:
            eventData(event);
            break;
        case MQTT_EVENT_ERROR:
            break;
    }
    
    
    return OK;
}

int publish(msg_mqtt_t * mqtt, uint8_t * data,  size_t len)
{
    mqtt->publish((handle_t *) mqtt->handle, "/topic/qos0", data, 0, 0, 0);
    return 0;
}
handle_t mqtt_start(msg_mqtt_settings_t * settings)
{
    const config_t mqtt_cfg = {
        .event_handle = mqtt_event_handler,
        .user_context = (void *) settings->mqtt,
        .host = settings->host,
        .port = settings->port,
        .client_id = settings->clientId,
        .username = settings->username,
    };

    handle_t client = settings->mqtt->init(&mqtt_cfg);
    settings->mqtt->start(client);
    return client;
}