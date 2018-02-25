#ifndef _MSG_TCPIP_H_
#define _MSG_TCPIP_H_

#include "msg_core.h"

messagingClient_t * createTcpIpClient(messagingSettings_t);

int tcpIpClientStart(messagingSettings_t settings);

#endif