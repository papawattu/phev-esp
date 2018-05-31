#ifndef _PHEV_CONTROLLER_H_
#define _PHEV_CONTROLLER_H_

#include "phev_core.h"
#include "msg_pipe.h"

#define MAX_QUEUED_COMMANDS 10
#define PHEV_OK 0

typedef struct phevCtx_t phevCtx_t;

typedef struct phevSettings_t
{
    messagingClient_t * in;
    messagingClient_t * out;
    msg_pipe_transformer_t inputTransformer;
    msg_pipe_transformer_t outputTransformer;
} phevSettings_t;

typedef enum phevEventType { CONNECT_REQUEST } phevEventType_t; 

struct phevCtx_t {    
    phevMessage_t * queuedCommands[MAX_QUEUED_COMMANDS];
    size_t queueSize;
    msg_pipe_ctx_t * pipe;
    msg_pipe_transformer_t outputTransformer;
}; 

typedef struct phevEvent_t
{
    phevEventType_t type;
} phevEvent_t;

int phev_controller_handleEvent(phevEvent_t * event);

phevCtx_t * phev_controller_init(phevSettings_t * settings);
void phev_controller_setCarConnectionConfig(const char * ssid, const char * password, phevConfig_t * config);
//void phev_controller_connect(phevCtx_t * ctx);

#endif

