#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "phev_core.h"
#include "msg_core.h"
#include "msg_utils.h"

const uint8_t allowedCommands[] = {0xf2, 0x2f, 0xf6, 0x6f, 0xf9, 0x9f};

phevMessage_t * phev_core_createMessage(uint8_t command, uint8_t type, uint8_t reg, uint8_t * data, size_t length)
{
    phevMessage_t * message = malloc(sizeof(phevMessage_t));

    message->command = command;
    message->type = type;
    message->reg = reg;
    message->length = length;
    message->data = malloc(message->length);
    memcpy(message->data, data, length);
    
    return message;
}

void phev_core_destroyMessage(phevMessage_t * message) 
{
    if(message == NULL) return;

    if(message->data != NULL)
    {
        free(message->data);
    }
    free(message);
}
int phev_core_validate_buffer(uint8_t * msg, size_t len)
{
    for(int i = 0;i < sizeof(allowedCommands); i++)
    {
        if(msg[0] == allowedCommands[i])
        {
            if((msg[1] + 2) > len)
            {
                return 0;  // length goes past end of message
            }
            return 1; //valid message
        }
    }
    return 0;  // invalid command
}
int phev_core_decodeMessage(const uint8_t *data, const size_t len, phevMessage_t *msg)
{
    if(phev_core_validate_buffer(data, len) != 0)
    {

        msg->command = data[0];
        msg->length = data[1];
        msg->type = data[2];
        msg->reg = data[3];
        msg->data = malloc(msg->length - 3);
        memcpy(msg->data, data + 4, msg->length - 3);
        msg->checksum = data[5 + msg->length];
        return msg->length + 2;
    } else {
        return 0;
    }
}
message_t * phev_core_extractMessage(const uint8_t *data, const size_t len)
{
    if(phev_core_validate_buffer(data, len) != 0)
    {
        message_t * message = malloc(sizeof(message_t));

        message->data = malloc(data[1] + 2);
        message->length = data[1] + 2;
        memcpy(message->data, data, message->length);
        return message;
    } else {
        return NULL;    
    }
}

int phev_core_encodeMessage(phevMessage_t *message,uint8_t ** data)
{
    uint8_t * d = malloc(message->length + 5);

    d[0] = message->command;
    d[1] = message->length + 3;
    d[2] = message->type;
    d[3] = message->reg;
    memcpy(d + 4, message->data, message->length);
    d[message->length + 4] = phev_core_checksum(d);

    *data = d;

    return d[1] + 2;
}

phevMessage_t *phev_core_message(uint8_t command, uint8_t type, uint8_t reg, uint8_t *data, size_t length)
{
    return phev_core_createMessage(command, type, reg, data, length);
}
phevMessage_t *phev_core_responseMessage(uint8_t command, uint8_t reg, uint8_t *data, size_t length)
{
    return phev_core_message(command, RESPONSE_TYPE, reg, data, length);
}
phevMessage_t *phev_core_requestMessage(uint8_t command, uint8_t reg, uint8_t *data, size_t length)
{
    return phev_core_message(command, REQUEST_TYPE, reg, data, length);
}
phevMessage_t *phev_core_commandMessage(uint8_t reg, uint8_t *data, size_t length)
{
    return phev_core_requestMessage(SEND_CMD, reg, data, length);
}
phevMessage_t *phev_core_simpleRequestCommandMessage(uint8_t reg, uint8_t value)
{
    const uint8_t data = value;
    return phev_core_requestMessage(SEND_CMD, reg, &data, 1);
}
phevMessage_t *phev_core_simpleResponseCommandMessage(uint8_t reg, uint8_t value)
{
    const uint8_t data = value;
    return phev_core_responseMessage(SEND_CMD, reg, &data, 1);
}
phevMessage_t *phev_core_ackMessage(uint8_t command, uint8_t reg)
{
    const uint8_t data = 0;
    return phev_core_responseMessage(command, reg, &data, 1);
}
phevMessage_t *phev_core_startMessage(uint8_t pos, uint8_t *mac)
{
    uint8_t * data = malloc(7);
    data[0] = pos;
    memcpy(data + 1,mac, 6);
    return phev_core_requestMessage(START_SEND, 0x01, data, 7);
}
message_t *phev_core_startMessageEncoded(uint8_t pos, uint8_t *mac)
{
    phevMessage_t * start = phev_core_startMessage(pos, mac);
    phevMessage_t * startaa = phev_core_simpleRequestCommandMessage(0xaa,0);
    message_t * message = msg_utils_concatMessages(
                                    phev_core_convertToMessage(start),
                                    phev_core_convertToMessage(startaa)
                                );
    phev_core_destroyMessage(start);
    phev_core_destroyMessage(startaa);
    return message;
}
phevMessage_t *phev_core_pingMessage(uint8_t number)
{
    const uint8_t data = 0;
    return phev_core_requestMessage(PING_SEND_CMD, number, &data, 1);
}
phevMessage_t *phev_core_responseHandler(phevMessage_t * message)
{
    uint8_t command = ((message->command & 0xf) << 4) | ((message->command & 0xf0) >> 4);
    return phev_core_ackMessage(command, message->reg); 
}

uint8_t phev_core_checksum(const uint8_t * data) 
{
    uint8_t b = 0;
    int j = data[1] + 2;
    for (int i = 0;; i++)
    {
      if (i >= j - 1) {
        return b;
      }
      b = (uint8_t)(data[i] + b);
    }
}
message_t * phev_core_convertToMessage(phevMessage_t *message)
{
    message_t * out = malloc(sizeof(message_t));   

    out->length = phev_core_encodeMessage(message, &out->data);

    return out;
}