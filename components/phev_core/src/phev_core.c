#include <stdlib.h>
#include <string.h>
#include "phev_core.h"

int phev_core_firstMessage(const uint8_t *data, phevMessage_t *msg)
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

int phev_core_encodeMessage(phevMessage_t *message,uint8_t ** data)
{
    uint8_t * d = malloc(message->length + 2);

    d[0] = message->command;
    d[1] = message->length;
    d[2] = message->type;
    d[3] = message->reg;
    memcpy(d + 4, message->data, message->length - 3);
    d[message->length + 1] = phev_core_checksum(d);

    *data = d;
    return message->length + 2;
}

phevMessage_t *phev_core_message(uint8_t command, uint8_t type, uint8_t reg, uint8_t *data, size_t length)
{
    phevMessage_t *message = malloc(sizeof(phevMessage_t));
    message->data = malloc(length);

    message->command = command;
    message->length = length + 3;
    message->type = type;
    message->reg = reg;
    memcpy(message->data, data, length);
    message->checksum = 0;

    return message;
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
phevMessage_t *phev_core_ackMessage(uint8_t reg)
{
    return phev_core_simpleResponseCommandMessage(reg, 0);
}
phevMessage_t *phev_core_startMessage(uint8_t *mac)
{
    return phev_core_requestMessage(START_SEND, 0x02, mac, 7);
}
phevMessage_t *phev_core_pingMessage(uint8_t *number)
{
    const uint8_t data = 0;
    return phev_core_requestMessage(PING_SEND_CMD, number, &data, 1);
}
phevMessage_t *phev_core_responseHandler(phevMessage_t * message)
{
    return phev_core_ackMessage(message->reg); 
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