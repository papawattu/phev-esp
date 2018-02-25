#include "msg_tcpip.h"
#ifndef TEST
#include "lwip/sockets.h"
#else
#include "sockets.h"
#endif

int tcpIpClientStart(messagingClient_t *client, messagingSettings_t settings) 
{
    struct sockaddr_in remote_ip;
    int _socket;
    bzero(&remote_ip, sizeof(struct sockaddr_in));
    remote_ip.sin_family = AF_INET;
    remote_ip.sin_port = htons(settings.port);

    inet_aton(settings.host, &(remote_ip.sin_addr));

    _socket = socket(PF_INET, SOCK_STREAM, 0);
    if (_socket == -1)
    {
        return -1;
    }

    if (connect(_socket, (struct sockaddr *)(&remote_ip), sizeof(struct sockaddr)) != 00)
    {
        return -1;
    }

    return 0;
}
int tcpipStart(messagingClient_t *client)
{

}
int tcpipStop(messagingClient_t *client)
{
    
}
int tcpipConnect(messagingClient_t *client)
{
    
}
void tcpipIncomingHandler(messagingClient_t *client, message_t message)
{
    
}
void tcpipOutgoingHandler(messagingClient_t *client, message_t  message)
{
    
}
messagingClient_t * createTcpIpClient(tcpipsettings_t settings)
{
    messagingSettings_t clientSettings;

    clientSettings.host = settings.host;
    clientSettings.port = settings.port;

    clientSettings.incomingHandler = tcpipIncomingHandler;
    clientSettings.outgoingHandler = tcpipOutgoingHandler;
    
    clientSettings.start = tcpipStart;
    clientSettings.stop = tcpipStop;
    clientSettings.connect = tcpipConnect;

    return createMessagingClient(clientSettings);

}