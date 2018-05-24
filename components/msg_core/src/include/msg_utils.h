#ifndef _MSG_UTILS_H_
#define _MSG_UTILS_H_

#include <stdint.h>
#include "msg_core.h"

//typedef message_t message_t;

message_t * msg_utils_createMsg(uint8_t * data, size_t length);

void msg_utils_destroyMsg(message_t * message);

message_t * msg_utils_copyMsg(message_t * message);

#endif