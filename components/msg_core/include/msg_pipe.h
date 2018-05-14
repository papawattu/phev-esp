#ifndef _MSG_PIPE_H_
#define _MSG_PIPE_H_

#include <stdlib.h>
#include "msg_core.h"

#define MAX_TRANSFORMERS 16
typedef struct msg_pipe_ctx_t msg_pipe_ctx_t;

typedef struct msg_pipe_transformer_t {
    message_t * (* input)(message_t *);
    message_t * (* output)(message_t *);
} msg_pipe_transformer_t;

typedef struct msg_pipe_ctx_t {
    messagingClient_t * in;
    messagingClient_t * out;
    void (* loop)(msg_pipe_ctx_t *ctx);
    msg_pipe_transformer_t  * transformers[MAX_TRANSFORMERS];
    int numTransformers;
} msg_pipe_ctx_t;

msg_pipe_ctx_t * msg_pipe(messagingClient_t *incoming, messagingClient_t *outgoing);

void msg_pipe_loop(msg_pipe_ctx_t * ctx);

void msg_pipe_add_transformer(msg_pipe_ctx_t * ctx, msg_pipe_transformer_t *transformer);

#endif
