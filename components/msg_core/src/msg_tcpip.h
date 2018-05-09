#ifndef _MSG_TCPIP_H_
#define _MSG_TCPIP_H_

#include "msg_core.h"

#define TCPIP_CLIENT_READ_BUF_SIZE 2048

typedef struct tcpIpSettings_t {
    char * host;
    uint16_t port;
} tcpIpSettings_t;
typedef struct tcpip_ctx_t {
    int socket;
    char * host;
    uint16_t port;
    uint8_t * readBuffer;
} tcpip_ctx_t;

messagingClient_t * msg_tcpip_createTcpIpClient(tcpIpSettings_t);

int msg_tcpip_start(messagingClient_t *client);
int msg_tcpip_stop(messagingClient_t *client);
int msg_tcpip_connect(messagingClient_t *client);

#endif