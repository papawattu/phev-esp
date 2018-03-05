#ifndef _MSG_GCP_MQTT_H_
#define _MSG_GCP_MQTT_H_
#include "sdkconfig.h"
typedef void *QueueHandle_t;
#include "mqtt_client.h"
#include "msg_core.h"

#define MSG_GCP_OK 0
#define MSG_GCP_FAIL -1

#define GCP_CLIENT_READ_BUF_SIZE 2048
//#define CONFIG_MQTT_SECURITY_ON

typedef struct gcpSettings_t
{
    char host[CONFIG_MQTT_MAX_HOST_LEN];
    uint32_t port;
    char client_id[CONFIG_MQTT_MAX_CLIENT_LEN];
    char username[CONFIG_MQTT_MAX_USERNAME_LEN];
    char password[CONFIG_MQTT_MAX_PASSWORD_LEN];
} gcpSettings_t;

typedef struct gcp_ctx_t
{
    int socket;
    char *host;
    uint16_t port;
    uint8_t *readBuffer;
    esp_mqtt_client_handle_t * client;
} gcp_ctx_t;

messagingClient_t *msg_gcp_createGcpClient(gcpSettings_t);

int msg_gcp_start(messagingClient_t *client);
int msg_gcp_stop(messagingClient_t *client);
int msg_gcp_connect(messagingClient_t *client);
void msg_gcp_connected_cb(void *self, void *params);
void msg_gcp_data_cb(void *self, void *params);
#endif