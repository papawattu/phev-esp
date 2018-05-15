#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "msg_gcp_mqtt.h"
#include "msg_mqtt.h"

//#include "jwt.h"

/*
void msg_gcp_data_cb(void * self, void * params)
{
    //esp_mqtt_client_handle_t *client = (esp_mqtt_client_handle_t *)self;
    //esp_mqtt_event_t *event_data = (esp_mqtt_event_t *)params;

    if (event_data->data_offset == 0)
    {

        char *topic = malloc(event_data->topic_length + 1);
        memcpy(topic, event_data->topic, event_data->topic_length);
        topic[event_data->topic_length] = 0;
        free(topic);
    }

    uint8_t *data = malloc(event_data->data_length + 1);
    memcpy(data, event_data->data, event_data->data_length);
    data[event_data->data_length] = 0;
    
    message_t message = {
        .data = data,
        .length = event_data->data_length
    };

    msg_core_call_subs((messagingClient_t *) client->settings->params, &message);
}
esp_mqtt_client_config_t mqttsettings = {
    .host = "mqtt.googleapis.com",
    .port = 8883,
    .client_id = "projects/phev-db3fa/locations/us-central1/registries/my-registry/devices/my-device",
    .username = "my-device",
    .clean_session = 0,
    .keepalive = 120,
    .connected_cb = msg_gcp_connected_cb,
    .disconnected_cb = NULL,
    .subscribe_cb = NULL,
    .publish_cb = NULL,
    .data_cb = msg_gcp_data_cb};

void msg_gcp_connected_cb(void *self, void *params)
{
//    esp_mqtt_client_handle_t * client = (esp_mqtt_client_handle_t *) self;
//    ((messagingClient_t *) client->settings->params)->connected = 1;

//    esp_mqtt_subscribe(client, "/devices/my-device/config", 0);
 
} 

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
} */
 
int msg_gcp_start(messagingClient_t *client) 
{
    return MSG_GCP_OK;
}
int msg_gcp_stop(messagingClient_t *client)
{
    return MSG_GCP_OK;
}

void msg_gcp_asyncIncomingHandler(messagingClient_t *client, message_t *message)
{
    msg_core_call_subs(client, message);
}

void msg_gcp_connected(mqtt_event_handle_t *event)
{    
    //subscribe((msg_mqtt_t *)((mqtt_event_t *)event)->user_context, "/devices/my-device/config");
}

int msg_gcp_connect(messagingClient_t *client)
{
    gcp_ctx_t * ctx = (gcp_ctx_t *) client->ctx;

    msg_mqtt_settings_t settings = {
        .host = ctx->host, //"mqtt.googleapis.com",
        .port = ctx->port, //8883,
        .clientId = ctx->clientId, //"projects/phev-db3fa/locations/us-central1/registries/my-registry/devices/my-device",
        .username = ctx->device, //"my-device",
        .password = ctx->createJwt(ctx->projectId), //"phev-db3fa")
        .mqtt = ctx->mqtt,
        .subscribed_cb = NULL,
        .connected_cb = msg_gcp_connected,
        .published_cb = NULL,
        .incoming_cb = msg_gcp_asyncIncomingHandler,
        .client = client,
        .transport = MSG_MQTT_TRANSPORT_OVER_SSL
    };
    
    ctx->mqtt->client = mqtt_start(&settings);

    return MSG_GCP_OK;
   
}
message_t * msg_gcp_incomingHandler(messagingClient_t *client)
{
    return NULL;
}
void msg_gcp_outgoingHandler(messagingClient_t *client, message_t *message)
{
    gcp_ctx_t * ctx = (gcp_ctx_t *) client->ctx;
    publish(ctx->mqtt, ctx->topic, message);
}
messagingClient_t * msg_gcp_createGcpClient(gcpSettings_t settings)
{
    messagingSettings_t clientSettings;
    
    gcp_ctx_t * ctx = malloc(sizeof(gcp_ctx_t));
    //msg_mqtt_t * mqtt_ctx = malloc(sizeof(msg_mqtt_t));

    ctx->host = settings.host;
    ctx->port = settings.port;
    ctx->device = settings.device;
    ctx->clientId = settings.clientId;
    ctx->topic = settings.topic;
    ctx->createJwt = settings.createJwt;
    ctx->projectId = settings.projectId;

    ctx->readBuffer = malloc(GCP_CLIENT_READ_BUF_SIZE);
    
    ctx->mqtt = settings.mqtt;

    clientSettings.incomingHandler = msg_gcp_incomingHandler;
    clientSettings.outgoingHandler = msg_gcp_outgoingHandler;
    
    clientSettings.start = msg_gcp_start;
    clientSettings.stop = msg_gcp_stop;
    clientSettings.connect = msg_gcp_connect;

    clientSettings.ctx = (void *) ctx;

    return msg_core_createMessagingClient(clientSettings);

} 