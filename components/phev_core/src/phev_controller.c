#include <stddef.h>
#include <string.h>
#include "phev_controller.h"
#include "msg_pipe.h"
#include "msg_utils.h"

message_t * phev_controller_responder(void * ctx, message_t * message)
{
    if(message != NULL) {

        phevMessage_t phevMsg;

        phev_core_decodeMessage(message->data, message->length, &phevMsg);
        if(phevMsg.command == RESP_CMD && phevMsg.type == REQUEST_TYPE)
        {
        
            return phev_core_convertToMessage(phev_core_responseHandler(&phevMsg));
        }
    }
    return NULL;

}
message_t * phev_controller_splitter(void * ctx, message_t * message)
{
    return phev_core_extractMessage(message->data, message->length);
}

message_t * phev_controller_outputChainInputTransformer(void * ctx, message_t * message)
{
    phevMessage_t * phevMessage = malloc(sizeof(phevMessage_t));

    int length = phev_core_decodeMessage(message->data, message->length, phevMessage);

    return phev_core_convertToMessage(phevMessage);
}

message_t * phev_controller_outputChainOutputTransformer(void * ctx, message_t * message)
{
    phevCtx_t * phevCtx = (phevCtx_t *) ctx;
    phevMessage_t * phevMessage = malloc(sizeof(phevMessage_t));
    phev_core_decodeMessage(message->data,message->length, phevMessage);

    return phevCtx->outputTransformer(ctx, phevMessage);
}

phevCtx_t * phev_controller_init(phevSettings_t * settings)
{
    phevCtx_t * ctx = malloc(sizeof(phevCtx_t));

    msg_pipe_chain_t * inputChain = malloc(sizeof(msg_pipe_chain_t));
    msg_pipe_chain_t * outputChain = malloc(sizeof(msg_pipe_chain_t));

    inputChain->inputTransformer = settings->inputTransformer;
    inputChain->aggregator = NULL;
    inputChain->splitter = NULL;
    inputChain->filter = NULL;
    inputChain->outputTransformer = NULL;
    inputChain->responder = NULL;
    
    outputChain->inputTransformer = phev_controller_outputChainInputTransformer;
    outputChain->aggregator = NULL;
    outputChain->splitter = phev_controller_splitter;
    outputChain->filter = NULL;
    outputChain->outputTransformer = phev_controller_outputChainOutputTransformer;
    outputChain->responder = phev_controller_responder;
    
    msg_pipe_settings_t pipe_settings = {
        .in = settings->in,
        .out = settings->out,
        .lazyConnect = 1,
        .user_context = ctx,
        .in_chain = inputChain,
        .out_chain = outputChain,
    };

    ctx->outputTransformer = settings->outputTransformer;
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
