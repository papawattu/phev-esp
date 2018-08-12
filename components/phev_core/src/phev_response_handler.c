#include "phev_response_handler.h"
#include "logger.h"
#ifdef __XTENSA__
#include "cJSON.h"
#else
#include <cjson/cJSON.h>
#endif

const static char *APP_TAG = "PHEV_RESPONSE_HANDLER";

message_t * phev_response_handler(void * ctx, phevMessage_t *message)
{
    LOG_V(APP_TAG,"START - responseHandler");
    
    char * output;

    if(message->type == RESPONSE_TYPE || message->command != 0x6f) 
    {
        return NULL;
    }
    
    cJSON * response = cJSON_CreateObject();
    if(response == NULL) 
    {
        return NULL;
    }
    cJSON * command = cJSON_CreateNumber(message->command);
    if(command == NULL) 
    {
        return NULL;
    }
    cJSON_AddItemToObject(response, "command", command);
    
    cJSON * type = NULL;
    if(message->type == REQUEST_TYPE)
    {
        type = cJSON_CreateString("request");
    } else {
        type = cJSON_CreateString("response");
    }
    if(type == NULL) 
    {
        return NULL;
    }
    
    cJSON_AddItemToObject(response, "type", type);
    
    cJSON * reg = cJSON_CreateNumber(message->reg);
    if(reg == NULL) 
    {
        return NULL;
    }
    cJSON_AddItemToObject(response, "register", reg);  

    cJSON * length = cJSON_CreateNumber(message->length);
    if(length == NULL) 
    {
        return NULL;
    }
    cJSON_AddItemToObject(response, "length", length);  

    cJSON * data = cJSON_CreateArray();
    if(data == NULL) 
    {
        return NULL;
    }
    cJSON_AddItemToObject(response, "data", data);  

    for(int i=0; i < message->length - 3; i++)
    {
        cJSON * item = cJSON_CreateNumber(message->data[i]);
        if (item == NULL)
        {
            return NULL;
        }
        cJSON_AddItemToArray(data, item);
    }
    
    output = cJSON_Print(response); 

    message_t * outputMessage = msg_utils_createMsg((uint8_t *) output, strlen(output));
    LOG_BUFFER_HEXDUMP(APP_TAG,outputMessage->data,outputMessage->length,LOG_DEBUG);
    cJSON_Delete(response);

    free(output);
    LOG_V(APP_TAG,"END - responseHandler");
    
    return outputMessage;
}
