#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "phev_controller.h"
#include "msg_pipe.h"
#include "msg_utils.h"
#include "msg_tcpip.h"

void phev_controller_preOutConnectHook(msg_pipe_ctx_t * pipe)
{
    phevCtx_t * ctx = (phevCtx_t *) pipe->user_context;
    ctx->startWifi(ctx->config->connectionConfig.carConnectionWifi.ssid,ctx->config->connectionConfig.carConnectionWifi.password);
    ((tcpip_ctx_t *) ctx->pipe->out->ctx)->host = ctx->config->connectionConfig.host;
    ((tcpip_ctx_t *) ctx->pipe->out->ctx)->port = ctx->config->connectionConfig.port;
    //ctx->pipe->in->connect(ctx->pipe->in);
    
}
message_t * phev_controller_input_responder(void * ctx, message_t * message) {
    
    return NULL;
}
message_t * phev_controller_responder(void * ctx, message_t * message)
{
    phevCtx_t * phevCtx = (phevCtx_t *) ctx;
    if(message != NULL) {

        phevMessage_t phevMsg;

        phev_core_decodeMessage(message->data, message->length, &phevMsg);

        if((phevMsg.type == REQUEST_TYPE)) // && (phevMsg.command == 0x6f))
        {
        
            return phev_core_convertToMessage(phev_core_responseHandler(&phevMsg));
        } else {
            if(phevMsg.command == 0x9f) {
                phevCtx->currentPing = (message->data[3] + 1) % 100;
                phevCtx->successfulPing = true;
            }
        }
        free(phevMsg.data);
        
    }
    return NULL;

}
messageBundle_t * phev_controller_splitter(void * ctx, message_t * message)
{
    message_t * out = phev_core_extractMessage(message->data, message->length);

    messageBundle_t * messages = malloc(sizeof(messageBundle_t));

    messages->numMessages = 0;
    messages->messages[messages->numMessages++] = out;
    
    int total = out->length;

    while(message->length > total)
    {
        out = phev_core_extractMessage(message->data + total, message->length - total);
        total += out->length;
        messages->messages[messages->numMessages++] = out;
        
    }
    return messages;
}

message_t * phev_controller_outputChainInputTransformer(void * ctx, message_t * message)
{
    phevMessage_t * phevMessage = malloc(sizeof(phevMessage_t));

    int length = phev_core_decodeMessage(message->data, message->length, phevMessage);
    msg_utils_destroyMsg(message);
    
    message_t * ret = phev_core_convertToMessage(phevMessage);

    phev_core_destroyMessage(phevMessage);
    return ret;
}

message_t * phev_controller_outputChainOutputTransformer(void * ctx, message_t * message)
{
    phevCtx_t * phevCtx = (phevCtx_t *) ctx;
    phevMessage_t * phevMessage = malloc(sizeof(phevMessage_t));
    phev_core_decodeMessage(message->data,message->length, phevMessage);
    msg_utils_destroyMsg(message);
    
    message_t * ret = phevCtx->outputTransformer(ctx, phevMessage);
    
    phev_core_destroyMessage(phevMessage);
    
    return ret;
}

void phev_controller_initState(phevState_t * state)
{
    state->connectedClients = 0;

}
void phev_controller_initConfig(phevConfig_t * config)
{
    phev_controller_initState(&config->state);
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
void phev_controller_sendMessage(phevCtx_t * ctx, message_t * message)
{
    ctx->pipe->out->publish(ctx->pipe->out, message);
}
void phev_controller_ping(phevCtx_t * ctx)
{
    if((ctx->currentPing % 30) == 0) 
    {
        time_t now;
        struct tm timeinfo;
        time(&now);
        localtime_r(&now, &timeinfo);
        
        const uint8_t pingTime[] = {
            timeinfo.tm_year - 100,
            timeinfo.tm_mon + 1,
            timeinfo.tm_mday,
            timeinfo.tm_hour,
            timeinfo.tm_min,
            timeinfo.tm_sec,
            1
        };
        phevMessage_t * dateCmd = phev_core_commandMessage(KO_WF_DATE_INFO_SYNC_SP,pingTime, sizeof(pingTime));
        message_t * message = phev_core_convertToMessage(dateCmd);
        phev_controller_sendMessage(ctx, message);
        msg_utils_destroyMsg(message);
        
        phev_core_destroyMessage(dateCmd);
    
    }
    ctx->successfulPing = false;
    phevMessage_t * ping = phev_core_pingMessage(ctx->currentPing);
    message_t * message = phev_core_convertToMessage(ping);
    phev_controller_sendMessage(ctx, message);
    msg_utils_destroyMsg(message);
    phev_core_destroyMessage(ping);
    
}
void phev_controller_resetPing(phevCtx_t * ctx)
{
    ctx->currentPing = 0;
}
void phev_controller_performUpdate(long buildNumber)
{

}
message_t * phev_controller_turnHeadLightsOn(phevCtx_t * ctx)
{
    phevMessage_t * headLightsOn = phev_core_simpleRequestCommandMessage(KO_WF_H_LAMP_CONT_SP, 1);
    message_t * message = phev_core_convertToMessage(headLightsOn);
    phev_core_destroyMessage(headLightsOn);
    
    return message;
}
messageBundle_t * phev_controller_configSplitter(void * ctx, message_t * message)
{
    phevConfig_t * config = phev_config_parseConfig(message->data);
    messageBundle_t * messages = malloc(sizeof(messageBundle_t));
    
    messages->numMessages = 0;

    if(phev_config_checkForFirmwareUpdate(&config->state)) 
    {
        phev_controller_performUpdate(config->updateConfig.latestBuild);
        return NULL; // shouldn't get here under normal conditions
    }
    
    if(phev_config_checkForConnection(&config->state)) 
    {
        // replace with real MAC

        uint8_t mac[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        
        messages->messages[messages->numMessages++] = phev_core_startMessageEncoded(2,mac);
    }

    if(phev_config_checkForHeadLightsOn(&config->state)) 
    {
        messages->messages[messages->numMessages++] = phev_controller_turnHeadLightsOn(ctx);
    }

    return messages;
} 

phevCtx_t * phev_controller_init(phevSettings_t * settings)
{
    phevCtx_t * ctx = malloc(sizeof(phevCtx_t));

    msg_pipe_chain_t * inputChain = malloc(sizeof(msg_pipe_chain_t));
    msg_pipe_chain_t * outputChain = malloc(sizeof(msg_pipe_chain_t));

    inputChain->inputTransformer = settings->inputTransformer;
    inputChain->aggregator = NULL;
    inputChain->splitter = phev_controller_configSplitter;
    inputChain->filter = NULL;
    inputChain->outputTransformer = NULL;
    inputChain->responder = phev_controller_input_responder;
    
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
        .preOutConnectHook = phev_controller_preOutConnectHook,
    };

    ctx->startWifi = settings->startWifi;
    ctx->outputTransformer = settings->outputTransformer;

    ctx->config = (phevConfig_t *) malloc(sizeof(phevConfig_t));

    ctx->pipe = msg_pipe(pipe_settings);
    ctx->queueSize = 0;
    ctx->currentPing = 0;
    ctx->successfulPing = false;

    phev_controller_initConfig(ctx->config);
    
    return ctx;
}
