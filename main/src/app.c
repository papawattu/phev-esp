#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "msg_mqtt.h"
#include "msg_gcp_mqtt.h"
#include "msg_pipe.h"
#include "msg_tcpip.h"
#include "msg_utils.h"

#include "phev_core.h"
#include "phev_controller.h"

int global_sock;

handle_t fake_mqtt_init(msg_mqtt_config_t * mqtt_cfg)
{
    return NULL;
}
void fake_mqtt_start()
{

}
void fake_mqtt_publish()
{

}
void fake_mqtt_subscribe()
{

}

msg_mqtt_t mqtt = {
    .init = fake_mqtt_init,
    .start = fake_mqtt_start,
    .publish = fake_mqtt_publish,
    .subscribe = fake_mqtt_subscribe,
};
char * createJwt()
{
    return "";
}

message_t * transformJSONToHex(void * ctx, message_t *message)
{
    return NULL;
}
message_t * transformHexToJSON(void * ctx, phevMessage_t *message)
{
    return NULL;
}
void wifi_conn_init(void)
{

}
void my_ms_to_timeval(int timeout_ms, struct timeval *tv)
{
    tv->tv_sec = timeout_ms / 1000;
    tv->tv_usec = (timeout_ms - (tv->tv_sec * 1000)) * 1000;
}
static int tcp_poll_read(int soc, int timeout_ms)
{
    fd_set readset;
    FD_ZERO(&readset);
    FD_SET(soc, &readset);
    struct timeval timeout;
    my_ms_to_timeval(timeout_ms, &timeout);
    return select(soc + 1, &readset, NULL, NULL, &timeout);
}
static int tcp_read(int soc, uint8_t *buffer, int len, int timeout_ms)
{
    int poll = -1;
    if ((poll = tcp_poll_read(soc, timeout_ms)) <= 0) {
        return poll;
    }
    int read_len = read(soc, buffer, len);
    if (read_len == 0) {
        return -1;
    }
    return read_len;
}

int logRead(int soc, uint8_t * buf, size_t len)
{

    int num = tcp_read(soc,buf,len,10000);

    if(num > 0) 
    {
    //    ESP_LOG_BUFFER_HEXDUMP(APP_TAG,buf,num,ESP_LOG_DEBUG);
    }
    return num;
}
int logWrite(int soc, uint8_t * buf, size_t len)
{
    //ESP_LOG_BUFFER_HEXDUMP(APP_TAG,buf,len,ESP_LOG_DEBUG);
    int num = write(soc,buf,len);
    //ESP_LOGD(APP_TAG, "Written %d bytes",num);
    
    return num;
}

int connectSocket(const char *host, uint16_t port) 
{
    printf("Connecting\n");
    struct sockaddr_in addr;
    /* set up address to connect to */
    memset(&addr, 0, sizeof(addr));
    //addr.sin_len = sizeof(addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(host);

      /* create the sockets */
    int sock = socket(AF_INET, SOCK_STREAM, 0);
  //  LWIP_ASSERT("sock >= 0", sock >= 0);

    if (sock == -1)
    {
        return -1;
    }
    int ret = connect(sock, (struct sockaddr *)(&addr), sizeof(addr));
    if(ret == -1)
    {
        return -1;
    }
    
    global_sock = sock;
    return sock;
}
int connectToCar(void)
{
    return connectSocket("192.168.1.64",8080);
}

phevCtx_t * createPhevController(void)
{
    gcpSettings_t inSettings = {
        .host = "mqtt.googleapis.com",
        .port = 8883,
        .clientId = "projects/phev-db3fa/locations/us-central1/registries/my-registry/devices/my-device2",
        .device = "my-device2",
        .createJwt = createJwt,
        .mqtt = &mqtt,
        .projectId = "phev-db3fa",
        .topic = "/devices/my-device2/events",
    }; 
    
    tcpIpSettings_t outSettings = {
        .connect = connectToCar, 
        .read = logRead,
        .write = logWrite,
    };
    
     
    phevSettings_t phev_settings = {
        .in = msg_gcp_createGcpClient(inSettings),
        .out = msg_tcpip_createTcpIpClient(outSettings),
        .inputTransformer = transformJSONToHex,
        .outputTransformer = transformHexToJSON,
        .startWifi = wifi_conn_init,
    };

    return phev_controller_init(&phev_settings);
}
void sendMessage(phevCtx_t * ctx, uint8_t * data, size_t length)
{
    mqtt_event_t event = {
        .event_id = MSG_MQTT_EVENT_DATA,
        .user_context = ((gcp_ctx_t *) ctx->pipe->in->ctx)->mqtt,
        .data = data,
        .data_len = length,
        .topic = "",
        .topic_len = 0,
    };
    mqtt_event_handler(&event);

}
int main()
{

        
    phevCtx_t * ctx = createPhevController();

    mqtt_event_t event = {
        .event_id = MSG_MQTT_EVENT_CONNECTED,
        .user_context = ((gcp_ctx_t *) ctx->pipe->in->ctx)->mqtt,
    };
    mqtt_event_handler(&event);
    sendMessage(ctx, "Hello", 5);
    
    while(1) 
    {
        //uint8_t data[] = {0x6f,0x04,0x00,0x27,0x00,0x9a};
        phev_controller_ping(ctx);
    //    phev_controller_ping(ctx);
    //    phev_controller_ping(ctx);
        msg_pipe_loop(ctx->pipe);
    }
    
}