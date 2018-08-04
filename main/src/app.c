#ifndef __XTENSA__
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include "msg_mqtt.h"
#include "msg_gcp_mqtt.h"
#include "msg_pipe.h"
#include "msg_tcpip.h"
#include "msg_utils.h"

#include "phev_core.h"
#include "phev_controller.h"

#define FILENAME "main/resources/config.json"

int global_sock;

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

    int num = tcp_read(soc,buf,len,100);

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
        //exit(1);
        return -1;
    }
    int ret = connect(sock, (struct sockaddr *)(&addr), sizeof(addr));
    if(ret == -1)
    {
        //exit(1);
        
        return -1;
    }
    
    global_sock = sock;
    return sock;
}
int connectToCar(void)
{
    return connectSocket("127.0.0.1",8080);
}
void outgoingHandler(messagingClient_t * client, message_t * message)
{
    printf("New Message\n");
}
message_t * incomingHandler(messagingClient_t *client) 
{
    return NULL;
}
int dummy_connect(messagingClient_t * client)
{
    client->connected = true;
    return 0;
}
messagingClient_t * setupAppMsgClient(void)
{
    messagingSettings_t settings = {
        .connect = dummy_connect,
        .incomingHandler = incomingHandler,
        .outgoingHandler = outgoingHandler,
    };

    messagingClient_t * client = msg_core_createMessagingClient(settings);
    return client;
}
char * createJwt()
{
    return "";
}

phevCtx_t * createPhevController(msg_mqtt_t mqtt)
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
#if defined(__linux__)
        .in = setupAppMsgClient(),
#else 
        .in = msg_gcp_createGcpClient(inSettings),
#endif
        .out = msg_tcpip_createTcpIpClient(outSettings),
        .startWifi = wifi_conn_init,
    };

    return phev_controller_init(&phev_settings);
}
int main()
{

    #if defined(__XTENSA__)
    printf("ESP...\n");
    #endif

    #if defined(__linux__)
    msg_mqtt_t mqtt = {};
    
    phevCtx_t * ctx = createPhevController(mqtt);

    printf("System is Linux...\nLoading config from file...");
    FILE * configFile = fopen(FILENAME,"r");
    if (configFile == NULL) {
        printf("Cannot open file");
        exit(-1);
    }
    
    struct stat st;
    stat(FILENAME, &st);
    size_t size = st.st_size;
    
    char * buffer = malloc(size);

    size_t num = fread(buffer,1,size,configFile);
    
    printf("Loaded...\n");

    phev_controller_setConfig(ctx, buffer);

    printf("Config set...\n");

    #endif

    printf("Build version :%d\n", ctx->config->updateConfig.currentBuild);
    printf("Latest build version :%d\n", ctx->config->updateConfig.latestBuild);
    
    printf("Starting message loop...\n");

    time_t now,prev;
    time(&now);
    time(&prev);
    printf("Time is %d\n", now);

    while(1) 
    {
        phev_controller_eventLoop(ctx);
        //sleep(1);

        
    }
    
}
#endif