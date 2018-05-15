#include <stdlib.h>
#include <string.h>

#include "msg_core.h"
#include "msg_mqtt.h"

void eventData(mqtt_event_handle_t event)
{
    char *topic = malloc(event->topic_len + 1);
    uint8_t *data = malloc(event->data_len + 1);
    
    memcpy(data, event->data, event->data_len);
    data[event->data_len] = 0;
    
    
    message_t * message = malloc(sizeof(message_t));

    message->data = data;
    message->length = event->data_len;
    
    ((msg_mqtt_t *) event->user_context)->incoming_cb(((msg_mqtt_t *) event->user_context)->client, message);

}

static err_t mqtt_event_handler(mqtt_event_handle_t event)
{
    msg_mqtt_t * mqtt = (msg_mqtt_t *) event->user_context;
    switch (event->event_id) {
        case MSG_MQTT_EVENT_CONNECTED:
            if(mqtt->connected_cb) mqtt->connected_cb(event);
            break;
        case MSG_MQTT_EVENT_DISCONNECTED:
            if(mqtt->disconnected_cb) mqtt->disconnected_cb(event);
            break;
        case MSG_MQTT_EVENT_SUBSCRIBED:
            if(mqtt->subscribed_cb) mqtt->subscribed_cb(event);
            break;
        case MSG_MQTT_EVENT_UNSUBSCRIBED:
            break;
        case MSG_MQTT_EVENT_PUBLISHED:
            if(mqtt->published_cb) mqtt->published_cb(event);
            break;
        case MSG_MQTT_EVENT_DATA:
            if(mqtt->incoming_cb) eventData(event);
            break;
        case MSG_MQTT_EVENT_ERROR:
            if(mqtt->error_cb) mqtt->error_cb(event);
            break;
    }
    
    return OK;
}

int publish(msg_mqtt_t * mqtt, topic_t topic, message_t *message)
{
    message_t *msg = msg_core_copyMessage(message);
    int id = mqtt->publish((handle_t *) mqtt->handle, topic, (const char *) msg->data, msg->length, 0, 0);
    
    return id;
}

void subscribe(msg_mqtt_t * mqtt, topic_t topic)
{
    mqtt->subscribe((handle_t *) mqtt->handle, topic, 0);
}
handle_t mqtt_start(msg_mqtt_settings_t * settings)
{
    const config_t mqtt_cfg = {
        //.uri = "mqtt://iot.eclipse.org",
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
    settings->mqtt->incoming_cb = settings->incoming_cb;
    settings->mqtt->connected_cb = settings->connected_cb;
    settings->mqtt->disconnected_cb = settings->disconnected_cb;
    settings->mqtt->subscribed_cb = settings->subscribed_cb;
    settings->mqtt->error_cb = settings->error_cb;
    settings->mqtt->published_cb = settings->published_cb;
    
    return client;
}