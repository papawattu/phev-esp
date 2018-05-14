#include <stdlib.h>
#include <string.h>

#include "msg_core.h"
#include "msg_mqtt.h"

void eventData(mqtt_event_handle_t event)
{
    char *topic = malloc(event->topic_len + 1);
    uint8_t *data = malloc(event->data_len + 1);
    
    memcpy(topic, event->topic, event->topic_len);
    topic[event->topic_len] = 0;
    free(topic);
    memcpy(data, event->data, event->data_len);
    data[event->data_len] = 0;
    
    message_t message = {
        .data = data,
        .length = event->data_len
    };

    ((msg_mqtt_t *) event->user_context)->incoming_cb(((msg_mqtt_t *) event->user_context)->client, &message);

}

static err_t mqtt_event_handler(mqtt_event_handle_t event)
{
    switch (event->event_id) {
        case MSG_MQTT_EVENT_CONNECTED:
            /*msg_id = ctx->subscribe(client, "/topic/qos0", 0);
            msg_id = ctx->subscribe(client, "/topic/qos1", 1);
            msg_id = ctx->unsubscribe(client, "/topic/qos1"); */
            break;
        case MSG_MQTT_EVENT_DISCONNECTED:
            break;
        case MSG_MQTT_EVENT_SUBSCRIBED:
            break;
        case MSG_MQTT_EVENT_UNSUBSCRIBED:
            break;
        case MSG_MQTT_EVENT_PUBLISHED:
            break;
        case MSG_MQTT_EVENT_DATA:
            eventData(event);
            break;
        case MSG_MQTT_EVENT_ERROR:
            break;
    }
    
    return OK;
}

int publish(msg_mqtt_t * mqtt, topic_t topic, message_t *message)
{
    //message_t *m = msg_core_copyMessage(message);
    int id = mqtt->publish((handle_t *) mqtt->handle, topic, (const char *) message->data, message->length, 0, 0);
    return id;
}
handle_t mqtt_start(msg_mqtt_settings_t * settings)
{
    const config_t mqtt_cfg = {
        .uri = "mqtt://iot.eclipse.org",
        .event_handle = mqtt_event_handler,
        .user_context = (void *) settings->mqtt,
        .host = settings->host,
        .port = settings->port,
        .client_id = settings->clientId,
        .username = settings->username,
    };

    handle_t client = settings->mqtt->init(&mqtt_cfg);
    settings->mqtt->start(client);
    settings->mqtt->handle = client;
    return client;
}