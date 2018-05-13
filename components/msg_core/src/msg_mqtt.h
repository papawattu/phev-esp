#ifndef MSG_MQTT_H_
#define MSG_MQTT_H_

#include "msg_core.h"
#include "mqtt_client.h"

typedef uint16_t err_t;
typedef void *handle_t;

typedef struct config_t 
{
    handle_t event_handle;
    void * user_context ;
    char * host;
    uint16_t port;
    char * client_id;
    char * username;
} config_t;

typedef struct msg_mqtt_t
{
    messagingClient_t * client;
    handle_t handle;
    handle_t (*init)(const config_t *config);
    err_t (*set_uri)(handle_t client, const char *uri);
    err_t (*start)(handle_t client);
    err_t (*stop)(handle_t client);
    err_t (*subscribe)(handle_t client, const char *topic, int qos);
    err_t (*unsubscribe)(handle_t client, const char *topic);
    int (*publish)(handle_t client, const char *topic, const char *data, int len, int qos, int retain);
    err_t (*destroy)(handle_t client);
} msg_mqtt_t;

typedef struct msg_mqtt_settings_t
{
    messagingClient_t * client;
    char *host;
    char *clientId;
    uint16_t port;
    char *username;
    char *password;
    msg_mqtt_t * mqtt;

} msg_mqtt_settings_t;

handle_t mqtt_start(msg_mqtt_settings_t * settings);

int publish(msg_mqtt_t *mqtt, uint8_t *data, size_t len);

#endif