#ifndef _MSG_TCPIP_H_
#define _MSG_TCPIP_H_

#include "msg_core.h"

typedef struct tcpipsettings_t {
    char * host;
    uint16_t port;
} tcpipsettings_t;

messagingClient_t * createTcpIpClient(tcpipsettings_t);

int tcpIpClientStart(messagingClient_t *client, messagingSettings_t settings);

#endif