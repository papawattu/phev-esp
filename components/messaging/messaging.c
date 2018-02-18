#include "stdlib.h"
#include "messaging.h"

int start(void)
{
    return 0;
}
int stop(void)
{
    return 0;
}
int publish(messageClient_t *client, message_t message)
{
    client->outgoingHandler(message);
    return 0;
}
int registerHandlers(messageClient_t *client, messageClientHandler_t incoming, messageClientHandler_t outgoing) 
{
    client->incomingHandler = incoming;
    client->outgoingHandler = outgoing;
    client->publish = publish;
    return 0;
}
int messagingClientInit(messageClient_t **client)
{
    (*client) = (messageClient_t *) malloc(sizeof(messageClient_t));
    (*client)->registerHandlers = registerHandlers;
    (*client)->start = start;
    (*client)->stop = stop;
    
    return 0;
}

