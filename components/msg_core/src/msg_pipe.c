#include "msg_pipe.h"

void msg_pipe_loop(msg_pipe_ctx_t * ctx)
{
    ctx->in->loop(ctx->in);
    ctx->out->loop(ctx->out);
}

message_t * msg_pipe_callInputTransformers(msg_pipe_ctx_t * ctx, message_t *message)
{
    message_t * last = message;
    if(ctx->in_inputTransformer != NULL) 
    {
        last = ctx->in_inputTransformer(last);
    }
    return last;
}
message_t * msg_pipe_callOutputTransformers(msg_pipe_ctx_t *ctx, message_t *message)
{
    message_t * last = message;
    if(ctx->out_outputTransformer != NULL) 
    {
        last = ctx->out_outputTransformer(last);
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

msg_pipe_ctx_t * msg_pipe(msg_pipe_settings_t settings) 
{
    msg_pipe_ctx_t * ctx = malloc(sizeof(msg_pipe_ctx_t));

    ctx->in = settings.in;
    ctx->out = settings.out;

    ctx->loop = msg_pipe_loop;

    ctx->in->subscribe(ctx->in, ctx, msg_pipe_inboundSubscription);
    ctx->out->subscribe(ctx->out, ctx, msg_pipe_outboundSubscription);

    ctx->in->start(ctx->in);
    
    ctx->in->connect(ctx->in);
    ctx->out->start(ctx->out);
    
    ctx->out->connect(ctx->out);
    
    ctx->in_splitter = settings.in_splitter;
    ctx->out_splitter = settings.out_splitter;
    
    
    ctx->in_outputTransformer = settings.in_outputTransformer;
    ctx->in_inputTransformer = settings.in_inputTransformer;

    ctx->out_outputTransformer = settings.out_outputTransformer;
    ctx->out_inputTransformer = settings.out_inputTransformer;

    return ctx;    
}
/*
void msg_pipe_add_transformer(msg_pipe_ctx_t * ctx, msg_pipe_transformer_t * transformer)
{
    if(ctx->numTransformers < MAX_TRANSFORMERS)
    {
        ctx->transformers[ctx->numTransformers++] = transformer; 
    }
} */