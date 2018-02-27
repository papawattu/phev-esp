#include "msg_tcpip.h"
#ifndef TEST
#include "lwip/sockets.h"
#else
#include "sockets.h"
#endif

int tcpipStart(messagingClient_t *client)
{
    
}
int tcpipStop(messagingClient_t *client)
{
    
}
int tcpipConnect(messagingClient_t *client)
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
        return -1;
    }
    if (connect(ctx->socket, (struct sockaddr *)(&remote_ip), sizeof(struct sockaddr)) != 0)
    {
        return -1;
    }
    client->connected = 1;
    return 0;    
}
message_t * tcpipIncomingHandler(messagingClient_t *client)
{
    tcpip_ctx_t * ctx = (tcpip_ctx_t *) client->ctx;
    int len = read(ctx->socket,ctx->readBuffer,TCPIP_CLIENT_READ_BUF_SIZE);

    if(len)
    {
        message_t * message = malloc(sizeof(message_t));
        message->data = malloc(len);
        memcpy(message->data, ctx->readBuffer,len);
        message->length = len;
        return message;
    } 
    return NULL;   
}
void tcpipOutgoingHandler(messagingClient_t *client, message_t *message)
{
    tcpip_ctx_t * ctx = (tcpip_ctx_t *) client->ctx;
    if(message->data && message->length) 
    {
        write(ctx->socket,message->data,message->length);
    }
}
messagingClient_t * createTcpIpClient(tcpIpSettings_t settings)
{
    messagingSettings_t clientSettings;
    
    tcpip_ctx_t * ctx = malloc(sizeof(tcpip_ctx_t));

    ctx->host = settings.host;
    ctx->port = settings.port;

    ctx->readBuffer = malloc(TCPIP_CLIENT_READ_BUF_SIZE);
    
    clientSettings.incomingHandler = tcpipIncomingHandler;
    clientSettings.outgoingHandler = tcpipOutgoingHandler;
    
    clientSettings.start = tcpipStart;
    clientSettings.stop = tcpipStop;
    clientSettings.connect = tcpipConnect;


    clientSettings.ctx = (void *) ctx;


    return createMessagingClient(clientSettings);

}