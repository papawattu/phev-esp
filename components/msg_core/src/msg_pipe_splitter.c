#include "msg_utils.h"
#include "msg_pipe.h"
#include "msg_pipe_splitter.h"


message_t * msg_pipe_concat(messageBundle_t * messages)
{
    size_t total = 0;
    if(messages->messages[0] == NULL) return NULL;
    
    uint8_t * data = malloc(messages->messages[0]->length);

    for(int i = 0;i < messages->numMessages; i++)
    {
        if(messages->messages[i] == NULL) break;
        data = realloc(data, total + messages->messages[i]->length);
        memcpy(data + total, messages->messages[i]->data, messages->messages[i]->length);
        total += messages->messages[i]->length;
    }

    if(total == 0) return NULL;

    message_t * message = malloc(sizeof(message_t));

    message->data = malloc(total);
    memcpy(message->data, data, total);
    message->length = total;
    free(data);
    return message;
}
message_t * msg_pipe_aggregrator(messageBundle_t * messages)
{
    return msg_pipe_concat(messages);
}
messageBundle_t * msg_pipe_splitter(msg_pipe_ctx_t *ctx, messagingClient_t * client, msg_pipe_chain_t * chain, message_t *message)
{
    return chain->splitter(ctx, message);   
}
