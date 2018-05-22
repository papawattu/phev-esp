#include <stdlib.h>
#include <string.h>
#include "phev_core.h"

int phev_core_firstMessage(const uint8_t * data, phevMessage_t *msg)
{
    msg->command = data[0];
    msg->length = data[1];
    msg->type = data[2];
    msg->reg = data[3];
    msg->data = malloc(msg->length - 3);
    memcpy(msg->data, data + 4, msg->length - 3);
    msg->checksum = data[5 + msg->length];
    return msg->length + 2;
}

uint8_t * phev_core_encodeMessage(phevMessage_t * message)
{
    uint8_t * data = malloc(message->length + 2);

    data[0] = message->command;
    data[1] = message->length;
    data[2] = message->type;
    data[3] = message->reg;
    memcpy(data + 4, message->data, message->length - 3);
    data[(5 + message->length) - 4] = message->checksum;

    return data;
}

 phevMessage_t * phev_core_simpleCommandMessage(uint8_t reg, uint8_t value) 
 {
     phevMessage_t * message = malloc(sizeof(phevMessage_t));
     message->data = malloc(1);

     message->command = SEND_CMD;
     message->length = DEFAULT_CMD_LENGTH;
     message->type = REQUEST_TYPE;
     message->reg = reg;
     *message->data = value;
     message->checksum = 0;

     return message;
 }
