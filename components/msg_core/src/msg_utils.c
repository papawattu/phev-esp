#include "msg_utils.h"

message_t * msg_utils_createMsg(uint8_t * data, size_t length)
{
    message_t * message = malloc(sizeof(message_t));

    message->data = malloc(length);
    memcpy(message->data, data, length);
    message->length = length;

    return message;
}

void msg_utils_destroyMsg(message_t * message)
{
    if(message != NULL)
    {
        if(message->data != NULL)
        {
            free(message->data);
        }
        free(message);
    }
}

message_t * msg_utils_copyMsg(message_t * message)
{
    message_t * out = malloc(sizeof(message_t));
    
    out->data = malloc(message->length);
    out->length = message->length;
    
    memcpy(out->data,message->data,message->length);
    return out;
}
