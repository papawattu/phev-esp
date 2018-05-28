#include <stddef.h>
#include <string.h>
#include "phev_controller.h"
#include "msg_pipe.h"

phevCtx_t * phev_controller_init(phevSettings_t * settings)
{
    phevCtx_t * ctx = malloc(sizeof(phevCtx_t));

    msg_pipe_chain_t * inputChain = malloc(sizeof(msg_pipe_chain_t));

    inputChain->inputTransformer = settings->inputTransformer;
    inputChain->aggregator = NULL;
    inputChain->splitter = NULL;
    inputChain->filter = NULL;
    inputChain->outputTransformer = NULL;
    inputChain->responder = NULL;
    
    msg_pipe_settings_t pipe_settings = {
        .in = settings->in,
        .out = settings->out,
        .lazyConnect = 1,
        .user_context = ctx,
        .in_chain = inputChain,
    };

    ctx->pipe = msg_pipe(pipe_settings);
    ctx->queueSize = 0;
    
    return ctx;
}

int phev_controller_handleEvent(phevEvent_t * event)
{
    return PHEV_OK;
}


void phev_controller_sendCommand(phevCtx_t * ctx, phevMessage_t * message) 
{
    int index = ctx->queueSize;
    ctx->queuedCommands[index] = malloc(sizeof(phevMessage_t));
    ctx->queuedCommands[index]->data = malloc(message->length - 2);
    memcpy(ctx->queuedCommands[index], message, sizeof(phevMessage_t));
    memcpy(ctx->queuedCommands[index]->data, message->data, message->length - 2); 

    ctx->queueSize ++;
}
void phev_controller_connect(phevCtx_t * ctx)
{ 
    uint8_t data[] = {0,0,0,0,0,0};

    phevMessage_t start = {
        .command    = START_SEND,
        .length     = 0x0a,
        .type       = REQUEST_TYPE,
        .reg        = 0x01,  
        .data       = &data,
    };
    phev_controller_sendCommand(ctx, &start);

    //phev_controller_waitForResponse(ctx);

} 
