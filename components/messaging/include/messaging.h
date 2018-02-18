#ifndef _MESSAGING_H_
#define _MESSAGING_H_

#include <stdint.h>


typedef struct messageClient_t messageClient_t;

typedef struct message_t message_t;
typedef uint8_t * data_t;

typedef void (* messageClientHandler_t)(message_t);

typedef uint8_t * data_t;

typedef struct message_t {
    data_t * data;
    size_t length;
}  message_t;

struct messageClient_t { 
    int (* start)(void);
    int (* stop)(void);

    int (* registerHandlers)(messageClient_t *client, messageClientHandler_t incoming, messageClientHandler_t outgoing);
    void (* incomingHandler)(message_t message);
    void (* outgoingHandler)(message_t message);
    int (* publish)(messageClient_t *client, message_t message);
};

int messagingClientInit(messageClient_t **); 

#endif
