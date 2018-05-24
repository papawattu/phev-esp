#ifndef _MSG_PIPE_H_
#define _MSG_PIPE_H_

#include <stdlib.h>
#include "msg_core.h"

#define MAX_TRANSFORMERS 16
typedef struct msg_pipe_ctx_t msg_pipe_ctx_t;

typedef message_t * (* msg_pipe_splitter_t)(message_t *);
typedef message_t * (* msg_pipe_filter_t)(message_t *);
typedef message_t * (* msg_pipe_responder_t)(message_t *);
typedef message_t * (* msg_pipe_aggregator_t)(message_t *);
typedef message_t * (* msg_pipe_transformer_t)(message_t *);


typedef struct msg_pipe_chain_t {
    msg_pipe_splitter_t splitter;
    msg_pipe_transformer_t inputTransformer;
    msg_pipe_filter_t filter;
    msg_pipe_responder_t responder;
    msg_pipe_transformer_t outputTransformer;
    msg_pipe_aggregator_t aggregator;
   
} msg_pipe_chain_t;
typedef struct msg_pipe_settings_t {
    messagingClient_t * in;
    messagingClient_t * out; 

    msg_pipe_chain_t * in_chain;
    msg_pipe_chain_t * out_chain;
    
 } msg_pipe_settings_t;

typedef struct msg_pipe_ctx_t {
    messagingClient_t * in;
    messagingClient_t * out;
    void (* loop)(msg_pipe_ctx_t *ctx);
    
    msg_pipe_chain_t * in_chain;
    msg_pipe_chain_t * out_chain;
    
 } msg_pipe_ctx_t;

msg_pipe_ctx_t * msg_pipe(msg_pipe_settings_t);

void msg_pipe_loop(msg_pipe_ctx_t * ctx);

message_t * msg_pipe_transformChain(msg_pipe_ctx_t * ctx, messagingClient_t * client, msg_pipe_chain_t * chain, message_t * message);

#endif
