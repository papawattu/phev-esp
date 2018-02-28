#include "stdlib.h"
#include "msg_core.h"

int start(messagingClient_t *client)
{
    return 0;
}
int stop(messagingClient_t *client)
{
    return 0;
}
int connect(messagingClient_t *client)
{
    return client->connect(client);
}
int publish(messagingClient_t *client, message_t *message)
{
    client->outgoingHandler(client, message);
    return 0;
}
int registerHandlers(messagingClient_t *client, messagingClientHandler_t incoming, messagingClientHandler_t outgoing) 
{
    client->incomingHandler = incoming;
    client->outgoingHandler = outgoing;
    client->publish = publish;
    return 0;
}

void loop(messagingClient_t *client) 
{
    int i;
    message_t *message = client->incomingHandler(client);

    if(message && client->numSubs)
    {
        for(i = 0;i < client->numSubs;i++)
        {
            client->subs[i][0](client, client->subs[i][1],message);
        }
    }

}
void subscribe(messagingClient_t *client, void * params, messagingSubscriptionCallback_t callback)
{
    if(client->numSubs < MAX_SUBSCRIPTIONS) 
    {
        client->subs[client->numSubs][0] = callback;
        client->subs[client->numSubs++][1] = params;
    }
}
int messagingClientInit(messagingClient_t **client)
{
    (*client) = (messagingClient_t *) malloc(sizeof(messagingClient_t));
    (*client)->start = start;
    (*client)->stop = stop;
    (*client)->connect = connect;
    (*client)->loop = loop;
    (*client)->subscribe = subscribe;

    (*client)->numSubs = 0;
    return 0;
}

messagingClient_t * createMessagingClient(messagingSettings_t settings)
{
    messagingClient_t * client;

    messagingClientInit(&client);
    registerHandlers(client, settings.incomingHandler,settings.outgoingHandler);

    client->start = settings.start;
    client->stop = settings.stop;
    client->connect = settings.connect;
    client->connected = 0;

    return client;
}