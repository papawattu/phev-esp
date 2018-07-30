#ifndef _PHEV_RESPONSE_HANDLER_H_
#define _PHEV_RESPONSE_HANDLER_H_
#include "msg_core.h"
#include "msg_utils.h"
#include "phev_core.h"

message_t * phev_response_handler(void * ctx, phevMessage_t *message);

#endif