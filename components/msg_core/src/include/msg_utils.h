#ifndef _MSG_UTILS_H_
#define _MSG_UTILS_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "msg_core.h"
#include "logger.h"
#ifndef __linux__
#include "esp_system.h"
#endif

const static char *MSG_UTILS_APP_TAG = "MSG_UTILS";

inline static message_t * msg_utils_createMsg(uint8_t * data, size_t length)
{
    LOG_V(MSG_UTILS_APP_TAG,"START - createMsg");
    message_t * message = malloc(sizeof(message_t));

#ifndef __linux__
    message->id = esp_random();
#else
    message->id = rand();
#endif
    message->data = malloc(length);
    memcpy(message->data, data, length);
    message->length = length;
    LOG_D(MSG_UTILS_APP_TAG,"Created Message ID %ul length %d",message->id,message->length);
    LOG_V(MSG_UTILS_APP_TAG,"END - createMsg");
    
    return message;
}
inline static void msg_utils_destroyMsg(message_t * message)
{
    LOG_V(MSG_UTILS_APP_TAG,"START - destroyMsg");
    
    if(message != NULL)
    {
        LOG_D(MSG_UTILS_APP_TAG,"Destroyed Message ID %ul length %d",message->id,message->length);
    
        if(message->data != NULL)
        {
            free(message->data);
        }
        free(message);
    }
    LOG_V(MSG_UTILS_APP_TAG,"END - destroyMsg");
    
}
inline static void msg_utils_destroyMsgBundle(messageBundle_t * messages) 
{
    LOG_V(MSG_UTILS_APP_TAG,"START - destroyMsgBundle");
    
    if(messages == NULL) return;
    
    for(int i=0;i<messages->numMessages;i++)
    {
        if(messages->messages[i] != NULL)
        {
            msg_utils_destroyMsg(messages->messages[i]);
        }
    }
    free(messages);
    LOG_V(MSG_UTILS_APP_TAG,"END - destroyMsgBundle");
    
}
inline static message_t * msg_utils_copyMsg(message_t * message)
{
    LOG_V(MSG_UTILS_APP_TAG,"START - copyMsg");
    
    message_t * out = msg_utils_createMsg(message->data,message->length);
    
    LOG_D(MSG_UTILS_APP_TAG,"Copied message %ul to new message %ul",message->id,out->id);
    
    LOG_V(MSG_UTILS_APP_TAG,"END - copyMsg");
    
    return out;
}

message_t * msg_utils_concatMessages(message_t * message1, message_t * message2);

#endif