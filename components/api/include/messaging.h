#ifndef _MESSAGING_H_
#define _MESSAGING_H_

#include <stdint.h>

typedef struct messagingClient_t messagingClient_t;

typedef struct message_t message_t;

typedef uint8_t * data_t;

typedef void (* messagingClientHandler_t)(message_t);

typedef void (* messagingSubscriptionCallback_t)(message_t);

typedef struct messagingSettings_t {
    char * host;
    int port;
    int (* start)(void);
    int (* stop)(void);
    int (* connect)(void);
    void (* incomingHandler)(message_t message);
    void (* outgoingHandler)(message_t message);
} messagingSettings_t;

typedef struct message_t {
    data_t * data;
    size_t length;
}  message_t;

struct messagingClient_t { 
    int (* start)(void);
    int (* stop)(void);
    int (* connect)(void);
    void (* incomingHandler)(message_t message);
    void (* outgoingHandler)(message_t message);
    int (* publish)(messagingClient_t *client, message_t message);
    int (* subscribe)(messagingClient_t *client, messagingSubscriptionCallback_t callback);
};

messagingClient_t * createMessagingClient(messagingSettings_t); 

#endif
