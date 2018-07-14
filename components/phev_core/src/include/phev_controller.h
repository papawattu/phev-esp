#ifndef _PHEV_CONTROLLER_H_
#define _PHEV_CONTROLLER_H_
#include <stdbool.h>
#include "phev_core.h"
#include "msg_pipe.h"

#define MAX_QUEUED_COMMANDS 10
#define PHEV_OK 0
#define IMAGE_PREFIX "firmware-"

#define KO_WF_DATE_INFO_SYNC_SP 5
#define KO_WF_H_LAMP_CONT_SP 10

typedef struct phevCtx_t phevCtx_t;

typedef void (* phevStartWifi_t)(const char * ssid, const char * password);
typedef struct phevSettings_t
{
    messagingClient_t * in;
    messagingClient_t * out;
    msg_pipe_transformer_t inputTransformer;
    msg_pipe_transformer_t outputTransformer;
    phevStartWifi_t startWifi;
} phevSettings_t;

typedef enum phevEventType { CONNECT_REQUEST } phevEventType_t; 

typedef struct phevState_t {
    bool headLightsOn;
    bool parkLightsOn;
    bool airConOn;
} phevState_t;

struct phevCtx_t {    
    phevConfig_t * config;
    phevMessage_t * queuedCommands[MAX_QUEUED_COMMANDS];
    size_t queueSize;
    msg_pipe_ctx_t * pipe;
    msg_pipe_transformer_t outputTransformer;
    phevStartWifi_t startWifi;
    int (* connect)(const char*, uint16_t);
    uint8_t currentPing;
    bool successfulPing;
    phevState_t state;
}; 

typedef struct phevEvent_t
{
    phevEventType_t type;
} phevEvent_t;

int phev_controller_handleEvent(phevEvent_t * event);

phevCtx_t * phev_controller_init(phevSettings_t * settings);
void phev_controller_setCarConnectionConfig(phevCtx_t *ctx, const char * ssid, const char * password, const char * host, const uint16_t port);
//void phev_controller_connect(phevCtx_t * ctx);
void phev_controller_ping(phevCtx_t * ctx);
void phev_controller_resetPing(phevCtx_t * ctx);

void phev_controller_setUpdateConfig(phevCtx_t * ctx, const char * ssid, const char * password, const char * host, const char * path, uint16_t port, int build);
message_t * phev_controller_input_responder(void * ctx, message_t * message);
void phev_controller_sendMessage(phevCtx_t * ctx, message_t * message);
message_t * phev_controller_turnHeadLightsOn(phevCtx_t * ctx);    
message_t * phev_controller_configSplitter(void * ctx, message_t * message);

#endif

