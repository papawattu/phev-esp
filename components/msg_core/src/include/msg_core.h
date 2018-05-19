#ifndef _MSG_CORE_H_
#define _MSG_CORE_H_

#define MAX_SUBSCRIPTIONS 10
#define SIMPLE_MESSAGE_SIZE 6
#include <stddef.h>
#include <stdint.h>

typedef struct messagingClient_t messagingClient_t;

typedef uint8_t data_t;

typedef struct message_t {
    data_t * data;
    size_t length;
}  message_t;
typedef void (* messagingClientHandler_t)(messagingClient_t *client, message_t *message);

typedef void (* messagingSubscriptionCallback_t)(messagingClient_t *client, void * params, message_t *message);

typedef struct messagingSettings_t {
    void * ctx;
    int (* start)(messagingClient_t *client);
    int (* stop)(messagingClient_t *client);
    int (* connect)(messagingClient_t *client);
    message_t * (* incomingHandler)(messagingClient_t *client);
    void (* outgoingHandler)(messagingClient_t *client, message_t *message);
} messagingSettings_t;
struct messagingClient_t {
    messagingSubscriptionCallback_t subs[MAX_SUBSCRIPTIONS][1];
    int numSubs;
    int connected;

    int (* start)(messagingClient_t *client);
    int (* stop)(messagingClient_t *client);
    int (* connect)(messagingClient_t *client);
    void (* loop)(messagingClient_t *client);
    messagingClient_t * (* incomingHandler)(messagingClient_t *client);
    void (* outgoingHandler)(messagingClient_t *client, message_t *message);
    int (* publish)(messagingClient_t *client, message_t *message);
    void (* subscribe)(messagingClient_t *client, void * params, messagingSubscriptionCallback_t callback);
    
    void * ctx;
};

messagingClient_t * msg_core_createMessagingClient(messagingSettings_t); 
void msg_core_call_subs(messagingClient_t *, message_t*);
message_t * msg_core_copyMessage(message_t *message);
#endif
