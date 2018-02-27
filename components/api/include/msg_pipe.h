#ifndef _MSG_PIPE_H_
#define _MSG_PIPE_H_

#include <stdlib.h>
#include "msg_core.h"

typedef struct msg_pipe_ctx_t msg_pipe_ctx_t;

typedef struct msg_pipe_ctx_t {
    messagingClient_t * in;
    messagingClient_t * out;
    void (* loop)(msg_pipe_ctx_t *ctx);
} msg_pipe_ctx_t;

msg_pipe_ctx_t * msg_pipe(messagingClient_t *incoming, messagingClient_t *outgoing);

void loop(msg_pipe_ctx_t * ctx);

#endif
