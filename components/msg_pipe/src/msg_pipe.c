#include "msg_pipe.h"

void loop(msg_pipe_ctx_t * ctx)
{
    ctx->in->loop(ctx->in);
    ctx->out->loop(ctx->out);
}

msg_pipe_ctx_t * msg_pipe(messagingClient_t *in, messagingClient_t *out) 
{
    msg_pipe_ctx_t * ctx = malloc(sizeof(msg_pipe_ctx_t));

    ctx->in = in;
    ctx->out = out;

    ctx->in->start(ctx->in);
    ctx->in->connect(ctx->in);

    ctx->out->start(ctx->out);
    ctx->out->connect(ctx->out);

    ctx->loop = loop;

    return ctx;    
}