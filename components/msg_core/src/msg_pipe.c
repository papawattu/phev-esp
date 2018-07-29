#include <stdio.h>
#include <string.h>
#include "msg_pipe.h"
#include "msg_pipe_splitter.h"
#include "msg_utils.h"

void msg_pipe_loop(msg_pipe_ctx_t * ctx)
{
    ctx->in->loop(ctx->in);
    ctx->out->loop(ctx->out);
}

message_t * msg_pipe_transformChain(msg_pipe_ctx_t * ctx, messagingClient_t * client, msg_pipe_chain_t * chain, message_t * message) 
{
    message_t * msg = message;

    if(chain->inputTransformer != NULL) 
    {
        msg = chain->inputTransformer(ctx->user_context, msg);
    }
    if(chain->filter != NULL)
    {
        if(!chain->filter(ctx->user_context, msg))
        {
            return NULL;
        }
    }
    if(chain->responder != NULL)
    {
        message_t * response = chain->responder(ctx->user_context, msg);
        if(response != NULL)
        {
            client->publish(client,response);
            msg_utils_destroyMsg(response);
        }
    }
    if(chain->outputTransformer != NULL)
    {
        msg = chain->outputTransformer(ctx->user_context, msg);
    }

    return msg;
}
message_t * msg_pipe_callInputTransformers(msg_pipe_ctx_t *ctx, message_t *message)
{
    if(ctx->in_chain->splitter != NULL)
    {
        return msg_pipe_splitter(ctx, ctx->in, ctx->in_chain, message);
    }  else {
        return msg_pipe_transformChain(ctx, ctx->in, ctx->in_chain, message);
    }
}
message_t * msg_pipe_callOutputTransformers(msg_pipe_ctx_t *ctx, message_t *message)
{
    if(ctx->out_chain->splitter != NULL)
    {
        return msg_pipe_splitter(ctx, ctx->out, ctx->out_chain, message);
    }  else {
        return msg_pipe_transformChain(ctx, ctx->out, ctx->out_chain, message);
    }
}
void msg_pipe_inboundSubscription(messagingClient_t *client, void * params, message_t * message)
{
    messagingClient_t *outboundClient = ((msg_pipe_ctx_t *) params)->out;
    message_t * out = message;
    msg_pipe_ctx_t * pipe = (msg_pipe_ctx_t *) params;
    
    if(pipe->in_chain != NULL)
    {
          out = msg_pipe_callInputTransformers(pipe, message);
    }
    if(outboundClient->connected == 0) 
    {
        msg_pipe_out_connect(pipe);
    }    
    if(out != NULL) 
    {
        outboundClient->publish(outboundClient, out);
        msg_utils_destroyMsg(out);
    }
}
void msg_pipe_outboundSubscription(messagingClient_t *client, void * params, message_t * message)
{
    messagingClient_t *inboundClient = ((msg_pipe_ctx_t *) params)->in;
    message_t * out = message;
    
    if(((msg_pipe_ctx_t *) params)->out_chain != NULL)
    {
        out = msg_pipe_callOutputTransformers((msg_pipe_ctx_t *) params, message);
    }
    
    if(out != NULL) 
    {
        inboundClient->publish(inboundClient, out);
        msg_utils_destroyMsg(out);
    }
    
}

int msg_pipe_in_connect(msg_pipe_ctx_t * ctx)
{
    if(ctx->preInConnectHook != NULL) ctx->preInConnectHook(ctx);
    return ctx->in->connect(ctx->in);
}
int msg_pipe_out_connect(msg_pipe_ctx_t * ctx)
{
    if(ctx->preOutConnectHook != NULL) ctx->preOutConnectHook(ctx);
    return ctx->out->connect(ctx->out);
}
msg_pipe_ctx_t * msg_pipe(msg_pipe_settings_t settings) 
{
    msg_pipe_ctx_t * ctx = malloc(sizeof(msg_pipe_ctx_t));

    ctx->in = settings.in;
    ctx->out = settings.out;

    ctx->in_chain = settings.in_chain;
    ctx->out_chain = settings.out_chain;

    ctx->user_context = settings.user_context;
    
    ctx->loop = msg_pipe_loop;

    ctx->preOutConnectHook = settings.preOutConnectHook;
    ctx->preInConnectHook = settings.preInConnectHook;


    ctx->in->subscribe(ctx->in, ctx, msg_pipe_inboundSubscription);
    ctx->out->subscribe(ctx->out, ctx, msg_pipe_outboundSubscription);

    ctx->in->start(ctx->in);
    
    msg_pipe_in_connect(ctx);
    ctx->out->start(ctx->out);
    
    if(!settings.lazyConnect) msg_pipe_out_connect(ctx);
    
    return ctx;    
}