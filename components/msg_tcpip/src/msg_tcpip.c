#include "msg_tcpip.h"
#ifndef TEST
#include "lwip/sockets.h"
#else
#include "sockets.h"
#endif

int msg_tcpip_start(messagingClient_t *client)
{
    return 0;
}
int msg_tcpip_stop(messagingClient_t *client)
{
    return 0;   
}
int msg_tcpip_connect(messagingClient_t *client)
{
    struct sockaddr_in remote_ip;
    tcpip_ctx_t *ctx = (tcpip_ctx_t *) client->ctx;
    bzero(&remote_ip, sizeof(struct sockaddr_in));
    remote_ip.sin_family = AF_INET;
    remote_ip.sin_port = htons(ctx->port);

    inet_aton(ctx->host, &(remote_ip.sin_addr));

    ctx->socket = socket(PF_INET, SOCK_STREAM, 0);
    if (ctx->socket == -1)
    {
        //log_error("Cannot create socket");
        return -1;
    }
    if (connect(ctx->socket, (struct sockaddr *)(&remote_ip), sizeof(struct sockaddr)) != 0)
    {
        //log_error("Cannot connect to socket");
        return -1;
    }
    //log_info("Connected");
    client->connected = 1;
    return 0;    
}
message_t * msg_tcpip_incomingHandler(messagingClient_t *client)
{
    tcpip_ctx_t * ctx = (tcpip_ctx_t *) client->ctx;
    int len = read(ctx->socket,ctx->readBuffer,TCPIP_CLIENT_READ_BUF_SIZE);

    if(len)
    {
        //char * str = malloc(sizeof("Received message " + len));
        //sprintf(str, "Received message %s",ctx->readBuffer);
        //log_info("Received message");
        
        message_t * message = malloc(sizeof(message_t));
        message->data = malloc(len);
        memcpy(message->data, ctx->readBuffer,len);
        message->length = len;
        return message;
    } 
    return NULL;   
}
void msg_tcpip_outgoingHandler(messagingClient_t *client, message_t *message)
{
    tcpip_ctx_t * ctx = (tcpip_ctx_t *) client->ctx;
    if(message->data && message->length) 
    {
        //log_info("Sending message");
        //log_info(message->data);
        
        write(ctx->socket,message->data,message->length);
    }
}
messagingClient_t * msg_tcpip_createTcpIpClient(tcpIpSettings_t settings)
{
    messagingSettings_t clientSettings;
    
    tcpip_ctx_t * ctx = malloc(sizeof(tcpip_ctx_t));

    ctx->host = settings.host;
    ctx->port = settings.port;

    ctx->readBuffer = malloc(TCPIP_CLIENT_READ_BUF_SIZE);
    
    clientSettings.incomingHandler = msg_tcpip_incomingHandler;
    clientSettings.outgoingHandler = msg_tcpip_outgoingHandler;
    
    clientSettings.start = msg_tcpip_start;
    clientSettings.stop = msg_tcpip_stop;
    clientSettings.connect = msg_tcpip_connect;

    clientSettings.ctx = (void *) ctx;

    return msg_core_createMessagingClient(clientSettings);

}