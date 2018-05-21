#ifndef _PHEV_CORE_H_
#define _PHEV_CORE_H_

#include <stdint.h>

#define REQUEST_TYPE 0
#define RESPONSE_TYPE 1

enum commands_t  {
    PING_SEND_CMD = 0xf9, 
    PING_RESP_CMD = 0x9f, 
    START_SEND = 0xf2, 
    START_RESP = 0x2f, 
    SEND_CMD = 0xf6,
    RESP_CMD = 0x6f 
};

typedef struct phevMessage_t {
    uint8_t command;
    uint8_t length;
    uint8_t type;
    uint8_t reg;
    uint8_t * data;
    uint8_t checksum;
} phevMessage_t;

int phev_core_firstMessage(const uint8_t * data, phevMessage_t *message);

uint8_t * phev_core_encodeMessage(phevMessage_t * message);

#endif