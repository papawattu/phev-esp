#ifndef MSG_MQTT_H_
#define MSG_MQTT_H_

#include "mqtt_client.h"
typedef struct msg_mqtt_ctx_t {
    esp_mqtt_client_handle_t (* esp_mqtt_client_init)(const esp_mqtt_client_config_t *config);
    esp_err_t (* esp_mqtt_client_set_uri)(esp_mqtt_client_handle_t client, const char *uri);
    esp_err_t (* esp_mqtt_client_start)(esp_mqtt_client_handle_t client);
    esp_err_t (* esp_mqtt_client_stop)(esp_mqtt_client_handle_t client);
    esp_err_t (* esp_mqtt_client_subscribe)(esp_mqtt_client_handle_t client, const char *topic, int qos);
    esp_err_t (* esp_mqtt_client_unsubscribe)(esp_mqtt_client_handle_t client, const char *topic);
    int (* esp_mqtt_client_publish)(esp_mqtt_client_handle_t client, const char *topic, const char *data, int len, int qos, int retain);
    esp_err_t (* esp_mqtt_client_destroy)(esp_mqtt_client_handle_t client);
} msg_mqtt_ctx_t;

void mqtt_start(msg_mqtt_ctx_t ctx);

#endif