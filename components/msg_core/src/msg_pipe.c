#include "msg_pipe.h"

void msg_pipe_loop(msg_pipe_ctx_t * ctx)
{
    ctx->in->loop(ctx->in);
    ctx->out->loop(ctx->out);
}

message_t * msg_pipe_callInputTransformers(msg_pipe_ctx_t * ctx, message_t *message)
{
    message_t * last = message;
    for(int i = 0;i < ctx->numTransformers;i++) {
        if(ctx->transformers[i]) 
        {   
            if(ctx->transformers[i]->input)
            {
                last = ctx->transformers[i]->input(last);
            }
        }
    }
    return last;
}
message_t * msg_pipe_callOutputTransformers(msg_pipe_ctx_t *ctx, message_t *message)
{
    message_t * last = message;
    for(int i = 0;i < ctx->numTransformers;i++) {
        if(ctx->transformers[i]) 
        {   
            if(ctx->transformers[i]->output)
            {
                last = ctx->transformers[i]->output(last);
                if(last == NULL) return NULL;
            }
        }
    }
    return last;
}
void msg_pipe_inboundSubscription(messagingClient_t *client, void * params, message_t * message)
{
    messagingClient_t *outboundClient = ((msg_pipe_ctx_t *) params)->out;

    message_t * out = msg_pipe_callInputTransformers((msg_pipe_ctx_t *) params, message);
    if(out) outboundClient->publish(outboundClient, out);
}
void msg_pipe_outboundSubscription(messagingClient_t *client, void * params, message_t * message)
{
    messagingClient_t *inboundClient = ((msg_pipe_ctx_t *) params)->in;

    message_t * out = msg_pipe_callOutputTransformers((msg_pipe_ctx_t *) params, message);
    if(out) inboundClient->publish(inboundClient, out);
}

msg_pipe_ctx_t * msg_pipe(messagingClient_t *in, messagingClient_t *out) 
{
    msg_pipe_ctx_t * ctx = malloc(sizeof(msg_pipe_ctx_t));

    ctx->in = in;
    ctx->out = out;

    ctx->loop = msg_pipe_loop;

    ctx->in->subscribe(ctx->in, ctx, msg_pipe_inboundSubscription);
    ctx->out->subscribe(ctx->out, ctx, msg_pipe_outboundSubscription);

    ctx->in->start(ctx->in);
    ctx->in->connect(ctx->in);

    ctx->out->start(ctx->out);
    ctx->out->connect(ctx->out);

    ctx->numTransformers = 0;

    return ctx;    
}

void msg_pipe_add_transformer(msg_pipe_ctx_t * ctx, msg_pipe_transformer_t * transformer)
{
    if(ctx->numTransformers < MAX_TRANSFORMERS)
    {
        ctx->transformers[ctx->numTransformers++] = transformer; 
    }
}