#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "msg_tcpip.h"

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
    tcpip_ctx_t * ctx = (tcpip_ctx_t *) client->ctx;
    int s = 0;
    if((s = ctx->connect(ctx->host,ctx->port))) 
    {
        ctx->socket = s;
        client->connected = 1;
    }

    return 0;
}
message_t *msg_tcpip_incomingHandler(messagingClient_t *client)
{
    if (client->connected)
    {
        tcpip_ctx_t *ctx = (tcpip_ctx_t *)client->ctx;
        int len = ctx->read(ctx->socket, ctx->readBuffer, TCPIP_CLIENT_READ_BUF_SIZE);

        if (len)
        {
            message_t *message = malloc(sizeof(message_t));
            message->data = malloc(len);
            memcpy(message->data, ctx->readBuffer, len);
            message->length = len;
            return message;
        }
    }
    return NULL;
}
void msg_tcpip_outgoingHandler(messagingClient_t *client, message_t *message)
{
    tcpip_ctx_t *ctx = (tcpip_ctx_t *)client->ctx;
    if (message->data && message->length)
    {
        ctx->write(ctx->socket, message->data, message->length);
    }
}
messagingClient_t *msg_tcpip_createTcpIpClient(tcpIpSettings_t settings)
{
    messagingSettings_t clientSettings;

    tcpip_ctx_t *ctx = malloc(sizeof(tcpip_ctx_t));

    ctx->read = settings.read;
    ctx->write = settings.write;
    ctx->connect = settings.connect;

    ctx->host = settings.host;
    ctx->port = settings.port;

    ctx->readBuffer = malloc(TCPIP_CLIENT_READ_BUF_SIZE);

    clientSettings.incomingHandler = msg_tcpip_incomingHandler;
    clientSettings.outgoingHandler = msg_tcpip_outgoingHandler;

    clientSettings.start = msg_tcpip_start;
    clientSettings.stop = msg_tcpip_stop;
    clientSettings.connect = msg_tcpip_connect;

    clientSettings.ctx = (void *)ctx;

    return msg_core_createMessagingClient(clientSettings);
}