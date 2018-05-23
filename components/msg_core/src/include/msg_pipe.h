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


typedef struct msg_pipe_settings_t {
    messagingClient_t * in;
    messagingClient_t * out; 

    msg_pipe_splitter_t in_splitter;
    msg_pipe_transformer_t in_inputTransformer;
    msg_pipe_filter_t in_filter;
    msg_pipe_responder_t in_responder;
    msg_pipe_aggregator_t in_aggregator;
    msg_pipe_transformer_t in_outputTransformer;

    msg_pipe_splitter_t out_splitter;
    msg_pipe_transformer_t out_inputTransformer;
    msg_pipe_filter_t out_filter;
    msg_pipe_responder_t out_responder;
    msg_pipe_aggregator_t out_aggregator;
    msg_pipe_transformer_t out_outputTransformer;
 } msg_pipe_settings_t;

typedef struct msg_pipe_ctx_t {
    messagingClient_t * in;
    messagingClient_t * out;
    void (* loop)(msg_pipe_ctx_t *ctx);
    
    msg_pipe_splitter_t in_splitter;
    msg_pipe_transformer_t in_inputTransformer;
    msg_pipe_filter_t in_filter;
    msg_pipe_responder_t in_responder;
    msg_pipe_aggregator_t in_aggregator;
    msg_pipe_transformer_t in_outputTransformer;

    msg_pipe_splitter_t out_splitter;
    msg_pipe_transformer_t out_inputTransformer;
    msg_pipe_filter_t out_filter;
    msg_pipe_responder_t out_responder;
    msg_pipe_aggregator_t out_aggregator;
    msg_pipe_transformer_t out_outputTransformer;
 } msg_pipe_ctx_t;

msg_pipe_ctx_t * msg_pipe(msg_pipe_settings_t);

void msg_pipe_loop(msg_pipe_ctx_t * ctx);

void msg_pipe_add_transformer(msg_pipe_ctx_t * ctx, msg_pipe_transformer_t *transformer);

#endif
