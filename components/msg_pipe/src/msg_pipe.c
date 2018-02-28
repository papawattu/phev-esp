#include "msg_pipe.h"

void msg_pipe_loop(msg_pipe_ctx_t * ctx)
{
    ctx->in->loop(ctx->in);
    ctx->out->loop(ctx->out);
}

void msg_inboundSubscription(messagingClient_t *client, void * params, message_t * message)
{
    messagingClient_t *outboundClient = (messagingClient_t *) params;

    outboundClient->publish(outboundClient, message);
}
void msg_outboundSubscription(messagingClient_t *client, void * params, message_t * message)
{
    messagingClient_t *inboundClient = (messagingClient_t *) params;

    inboundClient->publish(inboundClient, message);
}

msg_pipe_ctx_t * msg_pipe(messagingClient_t *in, messagingClient_t *out) 
{
    msg_pipe_ctx_t * ctx = malloc(sizeof(msg_pipe_ctx_t));

    ctx->in = in;
    ctx->out = out;

    ctx->loop = msg_pipe_loop;

    ctx->in->subscribe(ctx->in, ctx->out, msg_inboundSubscription);
    ctx->out->subscribe(ctx->out, ctx->in, msg_outboundSubscription);

    ctx->in->start(ctx->in);
    ctx->in->connect(ctx->in);

    ctx->out->start(ctx->out);
    ctx->out->connect(ctx->out);

    return ctx;    
}