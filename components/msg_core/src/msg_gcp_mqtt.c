#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "msg_gcp_mqtt.h"
#include "msg_mqtt.h"

//#include "jwt.h"

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
}
char *createJwt(const char *project_id)
{
    char iat_time[sizeof(time_t) * 3 + 2];
    char exp_time[sizeof(time_t) * 3 + 2];
    const uint8_t *key = (uint8_t *) priv_key;
    size_t key_len = sizeof(priv_key);
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
  //  ret = jwt_set_alg(jwt, JWT_ALG_RS256, key, key_len);
    if (ret)
    {
        printf("Error during set alg: %d", ret);
        return NULL;
    }
    out = jwt_encode_str(jwt);
    jwt_free(jwt); 
    return out;
} */
int msg_gcp_start(messagingClient_t *client) 
{
    return MSG_GCP_OK;
}
int msg_gcp_stop(messagingClient_t *client)
{
    return MSG_GCP_OK;
}

int msg_gcp_connect(messagingClient_t *client)
{
/*    const char *clientId = "projects/phev-db3fa/locations/us-central1/registries/my-registry/devices/my-device";
    const char *password = createJwt("phev-db3fa");

    if(password != NULL) 
    {
        strcpy(mqttsettings.client_id, clientId);
        strcpy(mqttsettings.password, password);
    
        mqttsettings.params = client;

       // mqtt_client * mqttclient = mqtt_start(&mqttsettings);

     //   ((gcp_ctx_t *) client->ctx)->client = mqttclient;
        return MSG_GCP_OK;
    }
*/    return MSG_GCP_FAIL;
}
message_t * msg_gcp_incomingHandler(messagingClient_t *client)
{
    return NULL;
}
void msg_gcp_outgoingHandler(messagingClient_t *client, message_t *message)
{
    ((gcp_ctx_t *) client->ctx)->mqttClient->publish(((gcp_ctx_t *) client->ctx)->mqttClient,  message->data, message->length);
}

messagingClient_t * msg_gcp_createGcpClient(gcpSettings_t settings)
{
    messagingSettings_t clientSettings;
    
    gcp_ctx_t * ctx = malloc(sizeof(gcp_ctx_t));

    ctx->host = settings.host;
    ctx->port = settings.port;
    ctx->mqttClient = settings.mqttClient;
    ctx->clientId = settings.clientId;
    ctx->jwt = settings.jwt;

    ctx->readBuffer = malloc(GCP_CLIENT_READ_BUF_SIZE);
    
    clientSettings.incomingHandler = msg_gcp_incomingHandler;
    clientSettings.outgoingHandler = msg_gcp_outgoingHandler;
    
    clientSettings.start = msg_gcp_start;
    clientSettings.stop = msg_gcp_stop;
    clientSettings.connect = msg_gcp_connect;

    clientSettings.ctx = (void *) ctx;

    return msg_core_createMessagingClient(clientSettings);

} 