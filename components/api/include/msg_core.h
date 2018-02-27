#ifndef _MSG_CORE_H_
#define _MSG_CORE_H_

#define MAX_SUBSCRIPTIONS 10
#include <stdint.h>

typedef struct messagingClient_t messagingClient_t;

typedef uint8_t data_t;

typedef struct message_t {
    data_t * data;
    size_t length;
}  message_t;
typedef void (* messagingClientHandler_t)(messagingClient_t *client, message_t *message);

typedef void (* messagingSubscriptionCallback_t)(messagingClient_t *client, message_t *message);

typedef struct messagingSettings_t {
    char * host;
    uint16_t port;
    int (* start)(messagingClient_t *client);
    int (* stop)(messagingClient_t *client);
    int (* connect)(messagingClient_t *client);
    message_t * (* incomingHandler)(messagingClient_t *client);
    void (* outgoingHandler)(messagingClient_t *client, message_t *message);
} messagingSettings_t;
struct messagingClient_t {
    char * host;
    uint16_t port; 

    messagingSubscriptionCallback_t subs[MAX_SUBSCRIPTIONS];
    int numSubs;

    int (* start)(messagingClient_t *client);
    int (* stop)(messagingClient_t *client);
    int (* connect)(messagingClient_t *client);
    void (* loop)(messagingClient_t *client);
    messagingClient_t * (* incomingHandler)(messagingClient_t *client);
    void (* outgoingHandler)(messagingClient_t *client, message_t *message);
    int (* publish)(messagingClient_t *client, message_t *message);
    void (* subscribe)(messagingClient_t *client, messagingSubscriptionCallback_t * callback);
};

messagingClient_t *createMessagingClient(messagingSettings_t); 

#endif
