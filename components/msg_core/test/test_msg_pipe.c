#include "unity.h"
#include "mock_msg_core.h"
#include "msg_pipe.h"
void setUp() 
{

}
void tearDown()
{

}

static int startInStubNum = 0;

int startInStub(messagingClient_t *client)
{
    startInStubNum ++;
}
static int connectInStubNum = 0;

int connectInStub(messagingClient_t *client)
{
    connectInStubNum ++;
}
static int startOutStubNum = 0;

int startOutStub(messagingClient_t *client)
{
    startOutStubNum ++;
}
static int connectOutStubNum = 0;

int connectOutStub(messagingClient_t *client)
{
    connectOutStubNum ++;
}
static int subscribeInStubNum = 0;

void subscribeInStub(messagingClient_t client, void * params, messagingSubscriptionCallback_t callback)
{
    subscribeInStubNum++;
}
static int subscribeOutStubNum = 0;

void subscribeOutStub(messagingClient_t client, void * params, messagingSubscriptionCallback_t callback)
{
    subscribeOutStubNum++;
} 
void test_should_call_start_incoming()
{
    messagingSettings_t settings;
    messagingClient_t mockIn;
    messagingClient_t mockOut;

    mockIn.start = startInStub;
    mockIn.connect = connectInStub;
    mockIn.subscribe = subscribeInStub;
    
    mockOut.start = startOutStub;
    mockOut.connect = connectOutStub;
    mockOut.subscribe = subscribeOutStub;
    
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockIn);
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockOut);
    
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = msg_core_createMessagingClient(settings),
    };

    startInStubNum = 0;
        
    msg_pipe(pipe_settings);
    
    TEST_ASSERT_EQUAL(1,startInStubNum);
    
}  
void test_should_call_connect_incoming()
{
    messagingSettings_t settings;
    messagingClient_t mockIn;
    messagingClient_t mockOut;
    
    mockIn.start = startInStub;
    mockIn.connect = connectInStub;
    mockIn.subscribe = subscribeInStub;
    
    mockOut.start = startOutStub;
    mockOut.connect = connectOutStub;
    mockOut.subscribe = subscribeOutStub;

    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockIn);
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockOut);
    
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = msg_core_createMessagingClient(settings),
    };

    connectInStubNum = 0;
        
    msg_pipe(pipe_settings);

    TEST_ASSERT_EQUAL(1,connectInStubNum);
} 
void test_should_call_start_outgoing()
{
    messagingSettings_t settings;
    messagingClient_t mockIn;
    messagingClient_t mockOut;

    mockIn.start = startInStub;
    mockIn.connect = connectInStub;
    mockIn.subscribe = subscribeInStub;
    
    mockOut.start = startOutStub;
    mockOut.connect = connectOutStub;
    mockOut.subscribe = subscribeOutStub;
    
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockIn);
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockOut);
    
    startOutStubNum = 0;

    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = msg_core_createMessagingClient(settings),
    };
        
    msg_pipe(pipe_settings);

    TEST_ASSERT_EQUAL(1,startOutStubNum);
    
} 
void test_should_call_connect_outgoing()
{
    messagingSettings_t settings;
    messagingClient_t mockIn;
    messagingClient_t mockOut;
    
    mockIn.start = startInStub;
    mockIn.connect = connectInStub;
    mockIn.subscribe = subscribeInStub;
    
    mockOut.start = startOutStub;
    mockOut.connect = connectOutStub;
    mockOut.subscribe = subscribeOutStub;

    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockIn);
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockOut);
    
    connectOutStubNum = 0;

    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = msg_core_createMessagingClient(settings),
    };
    
    msg_pipe(pipe_settings);

    TEST_ASSERT_EQUAL(1,connectOutStubNum);
} 
void test_should_not_call_connect_outgoing()
{
    messagingSettings_t settings;
    messagingClient_t mockIn;
    messagingClient_t mockOut;
    
    mockIn.start = startInStub;
    mockIn.connect = connectInStub;
    mockIn.subscribe = subscribeInStub;
    
    mockOut.start = startOutStub;
    mockOut.connect = connectOutStub;
    mockOut.subscribe = subscribeOutStub;

    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockIn);
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockOut);
    
    connectOutStubNum = 0;

    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = msg_core_createMessagingClient(settings),
        .lazyConnect = 1,
    };
    
    msg_pipe(pipe_settings);

    TEST_ASSERT_EQUAL(0,connectOutStubNum);
} 

static int dummyPublishCalled = 0;

void dummyPublish(messagingClient_t * client, message_t * message)
{

}
void test_should_connect_on_publish()
{
    messagingSettings_t settings;
    messagingClient_t mockIn;
    messagingClient_t mockOut;
    
    mockIn.start = startInStub;
    mockIn.connect = connectInStub;
    mockIn.subscribe = subscribeInStub;
    
    mockOut.start = startOutStub;
    mockOut.connect = connectOutStub;
    mockOut.subscribe = subscribeOutStub;
    mockOut.publish = dummyPublish;
    
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockOut);
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockIn);
    
    connectOutStubNum = 0;

    messagingClient_t * out = msg_core_createMessagingClient(settings);

    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = out,
        .lazyConnect = 1,
    };
    
    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);

    TEST_ASSERT_EQUAL(0, connectOutStubNum);
    const uint8_t data[] = {1,2,3,4};

    message_t * message = malloc(sizeof(message_t));
    message->data = malloc(4);
    memcpy(message->data, data, 4);
    message->length = 4;
    
    msg_pipe_inboundSubscription(out, ctx, message);

    TEST_ASSERT_EQUAL(1,connectOutStubNum);
}   
void test_should_set_in_and_out_clients()
{
    messagingSettings_t settings;
    messagingClient_t mockIn;
    messagingClient_t mockOut;
    
    mockIn.start = startInStub;
    mockIn.connect = connectInStub;
    mockIn.subscribe = subscribeInStub;
    
    mockOut.start = startOutStub;
    mockOut.connect = connectOutStub;
    mockOut.subscribe = subscribeOutStub;

    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockIn);
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockOut);
    
    connectOutStubNum = 0;

    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = msg_core_createMessagingClient(settings),
    };
      
    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);

    TEST_ASSERT_EQUAL(pipe_settings.in,ctx->in);
    TEST_ASSERT_EQUAL(pipe_settings.out,ctx->out);
} 
int loopInStubCalled = 0;
int loopOutStubCalled = 0;

void loopInStub(messagingClient_t * ctx)
{
    loopInStubCalled ++;
}
void loopOutStub(messagingClient_t * ctx)
{
    loopOutStubCalled ++;
} 
void test_should_call_loop_for_both_clients()
{
    messagingSettings_t settings;
    messagingClient_t mockIn;
    messagingClient_t mockOut;
    
    
    mockIn.start = startInStub;
    mockIn.connect = connectInStub;
    mockIn.subscribe = subscribeInStub;
    mockIn.loop = loopInStub;    
    
    mockOut.start = startOutStub;
    mockOut.connect = connectOutStub;
    mockOut.subscribe = subscribeOutStub;
    mockOut.loop = loopOutStub;

    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockIn);
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockOut);
    
    connectOutStubNum = 0;

    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = msg_core_createMessagingClient(settings),
    };
      
    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);

    ctx->loop(ctx);
    
    TEST_ASSERT_EQUAL(1,loopInStubCalled);
    TEST_ASSERT_EQUAL(1,loopOutStubCalled);
} 
void test_should_subscribe_both_clients()
{
    messagingSettings_t settings;
    messagingClient_t mockIn;
    messagingClient_t mockOut;
    
    mockIn.start = startInStub;
    mockIn.connect = connectInStub;
    mockIn.loop = loopInStub;
    mockIn.subscribe = subscribeInStub;
    
    
    mockOut.start = startOutStub;
    mockOut.connect = connectOutStub;
    mockOut.loop = loopOutStub;
    mockOut.subscribe = subscribeOutStub;
    
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockIn);
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockOut);
    
    subscribeInStubNum = 0;
    subscribeOutStubNum = 0;
    
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = msg_core_createMessagingClient(settings),
    };

    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);

    ctx->loop(ctx);

    TEST_ASSERT_EQUAL(1,subscribeInStubNum);
    TEST_ASSERT_EQUAL(1,subscribeOutStubNum);
    
} 
static int incomingPublishCalled = 0;
void incomingPublish(messagingClient_t *client, message_t * message)
{
    incomingPublishCalled ++;
    TEST_ASSERT_NOT_NULL(message);
} 
void test_should_publish_message_incoming()
{ 
    messagingSettings_t settings;
    messagingClient_t mockIn;
    messagingClient_t mockOut;
    
    mockIn.start = startInStub;
    mockIn.connect = connectInStub;
    mockIn.loop = loopInStub;
    mockIn.subscribe = subscribeInStub;
    mockIn.publish = incomingPublish;
        
    mockOut.start = startOutStub;
    mockOut.connect = connectOutStub;
    mockOut.loop = loopOutStub;
    mockOut.subscribe = subscribeOutStub;

    incomingPublishCalled = 0;
    
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockIn);
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockOut);
    
    messagingClient_t * in = msg_core_createMessagingClient(settings);
    msg_pipe_settings_t pipe_settings = {
        .in = in,
        .out = msg_core_createMessagingClient(settings),
    };

    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);

    const uint8_t data[] = {1,2,3,4};

    message_t * message = malloc(sizeof(message_t));
    message->data = malloc(4);
    memcpy(message->data, data, 4);
    message->length = 4;
    
    msg_pipe_outboundSubscription(in, ctx, message);

    ctx->loop(ctx);

    TEST_ASSERT_EQUAL(1, incomingPublishCalled);
    
} 
static int outgoingPublishCalled = 0;
void outgoingPublish(messagingClient_t *client, message_t * message)
{
    outgoingPublishCalled ++;
    TEST_ASSERT_NOT_NULL(message);
}
void test_should_publish_message_outgoing()
{ 
    messagingSettings_t settings;
    messagingClient_t mockIn;
    messagingClient_t mockOut;
    
    mockIn.start = startInStub;
    mockIn.connect = connectInStub;
    mockIn.loop = loopInStub;
    mockIn.subscribe = subscribeInStub;
        
    mockOut.start = startOutStub;
    mockOut.connect = connectOutStub;
    mockOut.loop = loopOutStub;
    mockOut.subscribe = subscribeOutStub;
    mockOut.publish = outgoingPublish;

    outgoingPublishCalled = 0;
    
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockOut);
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockIn);
    
    messagingClient_t * out = msg_core_createMessagingClient(settings);
    
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = out,

    };

    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);
    
    const uint8_t data[] = {1,2,3,4};

    message_t * message = malloc(sizeof(message_t));
    message->data = malloc(4);
    memcpy(message->data, data, 4);
    message->length = 4;
    
    msg_pipe_inboundSubscription(out, ctx, message);
    
    ctx->loop(ctx);
    
    TEST_ASSERT_EQUAL(1, outgoingPublishCalled);
    
} 
static int inputTransformCalled = 0;
message_t * inputTransform(message_t * message) 
{
    inputTransformCalled ++;
    return message;
} 
void test_should_set_in_input_transformer_in_settings()
{
    messagingSettings_t settings;
    messagingClient_t mockIn;
    messagingClient_t mockOut;
        
    mockIn.start = startInStub;
    mockIn.connect = connectInStub;
    mockIn.subscribe = subscribeInStub;
    mockIn.loop = loopInStub;    
    
    mockOut.start = startOutStub;
    mockOut.connect = connectOutStub;
    mockOut.subscribe = subscribeOutStub;
    mockOut.loop = loopOutStub;

    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockIn);
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockOut);
        
    msg_pipe_chain_t chain = {
        .inputTransformer = inputTransform,
    };    
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = msg_core_createMessagingClient(settings),
        .in_chain = &chain,
    };

    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);
    
    TEST_ASSERT_EQUAL(inputTransform,ctx->in_chain->inputTransformer);
} 
void test_should_set_out_input_transformer_in_settings()
{
    messagingSettings_t settings;
    messagingClient_t mockIn;
    messagingClient_t mockOut;
        
    mockIn.start = startInStub;
    mockIn.connect = connectInStub;
    mockIn.subscribe = subscribeInStub;
    mockIn.loop = loopInStub;    
    
    mockOut.start = startOutStub;
    mockOut.connect = connectOutStub;
    mockOut.subscribe = subscribeOutStub;
    mockOut.loop = loopOutStub;

    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockIn);
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockOut);
    
    inputTransformCalled = 0;
    msg_pipe_chain_t chain = {
        .inputTransformer = inputTransform,
    };

    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = msg_core_createMessagingClient(settings),
        .out_chain = &chain,
    };

    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);
    
    TEST_ASSERT_EQUAL(inputTransform,ctx->out_chain->inputTransformer);
}
 
static int outputTransformCalled = 0;
message_t * outputTransform(message_t * message) 
{
    outputTransformCalled ++;
    return message;
} 
void test_should_set_out_output_transformer()
{
    messagingSettings_t settings;
    messagingClient_t mockIn;
    messagingClient_t mockOut;   
    
    mockIn.start = startInStub;
    mockIn.connect = connectInStub;
    mockIn.subscribe = subscribeInStub;
    mockIn.loop = loopInStub;    
    
    mockOut.start = startOutStub;
    mockOut.connect = connectOutStub;
    mockOut.subscribe = subscribeOutStub;
    mockOut.loop = loopOutStub;

    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockIn);
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockOut);
    
    outputTransformCalled = 0;
    msg_pipe_chain_t chain = {
        .outputTransformer = outputTransform,
    };

    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = msg_core_createMessagingClient(settings),
        .out_chain = &chain,
    };

    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);
    TEST_ASSERT_EQUAL(outputTransform,ctx->out_chain->outputTransformer);
} 
void test_should_set_in_output_transformer()
{
    messagingSettings_t settings;
    messagingClient_t mockIn;
    messagingClient_t mockOut;   
    
    mockIn.start = startInStub;
    mockIn.connect = connectInStub;
    mockIn.subscribe = subscribeInStub;
    mockIn.loop = loopInStub;    
    
    mockOut.start = startOutStub;
    mockOut.connect = connectOutStub;
    mockOut.subscribe = subscribeOutStub;
    mockOut.loop = loopOutStub;

    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockIn);
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockOut);
    
    outputTransformCalled = 0;
    msg_pipe_chain_t chain = {
        .outputTransformer = outputTransform,
    };

    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = msg_core_createMessagingClient(settings),
        .in_chain = &chain,
    };

    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);
    TEST_ASSERT_EQUAL(outputTransform,ctx->in_chain->outputTransformer);
}  
static int splitterCalled = 0;
message_t * splitter_no_message(message_t * message)
{
    splitterCalled ++;
    return NULL;
} 
void test_should_set_up_in_splitter()
{
    messagingSettings_t settings;
    messagingClient_t mockIn;
    messagingClient_t mockOut;   
    
    mockIn.start = startInStub;
    mockIn.connect = connectInStub;
    mockIn.subscribe = subscribeInStub;
    mockIn.loop = loopInStub;    
    
    mockOut.start = startOutStub;
    mockOut.connect = connectOutStub;
    mockOut.subscribe = subscribeOutStub;
    mockOut.loop = loopOutStub;

    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockIn);
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockOut);
    
    splitterCalled = 0;
    
    msg_pipe_chain_t chain = {
        .splitter = splitter_no_message,
    };

    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = msg_core_createMessagingClient(settings),
        .in_chain = &chain,
    };

    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);
    TEST_ASSERT_EQUAL(splitter_no_message,ctx->in_chain->splitter);
}  
void test_should_set_up_out_splitter()
{
    messagingSettings_t settings;
    messagingClient_t mockIn;
    messagingClient_t mockOut;   
    
    mockIn.start = startInStub;
    mockIn.connect = connectInStub;
    mockIn.subscribe = subscribeInStub;
    mockIn.loop = loopInStub;    
    
    mockOut.start = startOutStub;
    mockOut.connect = connectOutStub;
    mockOut.subscribe = subscribeOutStub;
    mockOut.loop = loopOutStub;

    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockIn);
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockOut);
    
    splitterCalled = 0;
    msg_pipe_chain_t chain = {
        .splitter = splitter_no_message,
    };

    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = msg_core_createMessagingClient(settings),
        .out_chain = &chain,
    };

    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);
    TEST_ASSERT_EQUAL(splitter_no_message,ctx->out_chain->splitter);
} 
void test_should_call_out_splitter()
{
    messagingSettings_t settings;
    messagingClient_t mockIn;
    messagingClient_t mockOut;   
    
    mockIn.start = startInStub;
    mockIn.connect = connectInStub;
    mockIn.subscribe = subscribeInStub;
    mockIn.loop = loopInStub;    
    
    mockOut.start = startOutStub;
    mockOut.connect = connectOutStub;
    mockOut.subscribe = subscribeOutStub;
    mockOut.loop = loopOutStub;

    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockIn);
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockOut);
    
    splitterCalled = 0;
    msg_pipe_chain_t chain = {
        .splitter = splitter_no_message,
    };

    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = msg_core_createMessagingClient(settings),
        .out_chain = &chain,
    };

    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);
    
    const uint8_t data[] = {1,2,3,4};
    message_t message = {
        .data = &data,
        .length = 4,
    };
    msg_pipe_outboundSubscription(ctx->out, (void *) ctx, &message);
    
    TEST_ASSERT_EQUAL(1,splitterCalled);
} 
static int splitterOneMessageCalled = 0;
message_t * splitter_one_message(message_t * message)
{
    const uint8_t data[] = {1,2,3,4};

    splitterOneMessageCalled ++;
    
    message_t * msg = malloc(sizeof(message_t));
    msg->data = malloc(4);
    memcpy(msg->data, data, 4);
    msg->length = 4;
    
    return msg;
}

static int splitterTwoMessagesCalled = 0;

message_t * splitter_two_messages(message_t * message)
{
    const uint8_t data[] = {1,2,3,4};
    const uint8_t data2[] = {5,6,7,8};

    if(splitterTwoMessagesCalled == 0) 
    {
        message_t * msg = malloc(sizeof(message_t));
        msg->data = malloc(4);
        memcpy(msg->data, data, 4);    
        msg->length = 4;
        splitterTwoMessagesCalled =1;
    } else {
        message_t * msg = malloc(sizeof(message_t));
        msg->data = malloc(4);
        memcpy(msg->data, data2, 4);    
        msg->length = 4;
        splitterTwoMessagesCalled =2;
        return msg;
    }
} /* 
void test_should_call_out_splitter_two_messages()
{
    messagingSettings_t settings;
    messagingClient_t mockIn;
    messagingClient_t mockOut;   
    
    mockIn.start = startInStub;
    mockIn.connect = connectInStub;
    mockIn.subscribe = subscribeInStub;
    mockIn.loop = loopInStub;    
    
    mockOut.start = startOutStub;
    mockOut.connect = connectOutStub;
    mockOut.subscribe = subscribeOutStub;
    mockOut.loop = loopOutStub;

    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockIn);
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockOut);

    msg_pipe_chain_t chain = {
        .splitter = splitter_two_messages,
    };    
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = msg_core_createMessagingClient(settings),
        .out_chain = &chain,
    };

    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);
    
    const uint8_t data[] = {1,2,3,4,5,6,7,8};

    message_t * message = malloc(sizeof(message_t));
    message->data = malloc(8);
    memcpy(message->data, data, 8);
    message->length = 8;

    splitterTwoMessagesCalled = 0;
    msg_pipe_outboundSubscription(ctx->out, (void *) ctx, message);
    
    TEST_ASSERT_EQUAL(2,splitterTwoMessagesCalled);
}
void splitter_two_messages_publish(messagingClient_t * client, message_t * message) 
{
    const uint8_t data[] = {1,2,3,4,5,6,7,8};
    TEST_ASSERT_EQUAL(8, message->length);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(&data, message->data,8);
} 
void test_should_call_out_splitter_two_messages_and_get_back_both()
{
    messagingSettings_t settings;
    messagingClient_t mockIn;
    messagingClient_t mockOut;   
    
    mockIn.start = startInStub;
    mockIn.connect = connectInStub;
    mockIn.subscribe = subscribeInStub;
    mockIn.loop = loopInStub;  
    mockIn.publish = splitter_two_messages_publish;  
    
    mockOut.start = startOutStub;
    mockOut.connect = connectOutStub;
    mockOut.subscribe = subscribeOutStub;
    mockOut.loop = loopOutStub;

    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockIn);
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockOut);
    
    msg_pipe_chain_t chain = {
        .splitter = splitter_two_messages,
    };
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = msg_core_createMessagingClient(settings),
        .out_chain = &chain,
    };

    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);
    
    const uint8_t data[] = {1,2,3,4,5,6,7,8};
    message_t message = {
        .data = &data,
        .length = 8,
    };
    splitterTwoMessagesCalled = 0;
    
    msg_pipe_outboundSubscription(ctx->out, (void *) ctx, &message);
    
    TEST_ASSERT_EQUAL(2,splitterTwoMessagesCalled);
} 

static int all_splitterCalled = 0;
static int all_inputTransformerCalled = 0;
static int all_filterCalled = 0;
static int all_responderCalled = 0;
static int all_outputTransformerCalled = 0;
static int all_aggregatorCalled = 0;


message_t * all_splitter(void * ctx, message_t * message) 
{
    all_splitterCalled ++;
    TEST_ASSERT_NOT_NULL(ctx);
    TEST_ASSERT_EQUAL_STRING("Context", ctx);
    return message;
}
message_t * all_inputTransformer(void * ctx, message_t * message)
{
    all_inputTransformerCalled ++;
    TEST_ASSERT_NOT_NULL(ctx);
    TEST_ASSERT_EQUAL_STRING("Context", ctx);
    return message;
}
int all_filter(void * ctx, message_t * message)
{
    all_filterCalled ++;
    TEST_ASSERT_NOT_NULL(ctx);
    TEST_ASSERT_EQUAL_STRING("Context", ctx);
    return 1;
}
message_t * all_responder(void * ctx, message_t * message)
{
    all_responderCalled ++;
    TEST_ASSERT_NOT_NULL(ctx);
    TEST_ASSERT_EQUAL_STRING("Context", ctx);
    return message;
}
message_t * all_outputTransformer(void * ctx, message_t * message)
{
    all_outputTransformerCalled ++;
    TEST_ASSERT_NOT_NULL(ctx);
    TEST_ASSERT_EQUAL_STRING("Context", ctx);
    return message;
}
message_t * all_aggregator(void * ctx, message_t * message)
{
    all_aggregatorCalled ++;
    TEST_ASSERT_NOT_NULL(ctx);
    TEST_ASSERT_EQUAL_STRING("Context", ctx);
    return message;
}

void all_publish(messagingClient_t * client, message_t * message) 
{
    const uint8_t data[] = {1,2,3,4,5,6,7,8};
    TEST_ASSERT_NOT_NULL(client);
    TEST_ASSERT_EQUAL(8, message->length);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(&data, message->data,8);
} 
    
void test_should_call_all_output_transformers()
{
    messagingSettings_t settings;
    messagingClient_t mockIn;
    messagingClient_t mockOut;   
    
    mockIn.start = startInStub;
    mockIn.connect = connectInStub;
    mockIn.subscribe = subscribeInStub;
    mockIn.loop = loopInStub;  
    mockIn.publish = splitter_two_messages_publish;  
    
    mockOut.start = startOutStub;
    mockOut.connect = connectOutStub;
    mockOut.subscribe = subscribeOutStub;
    mockOut.loop = loopOutStub;
    mockOut.publish = all_publish;

    all_splitterCalled = 0;
    all_inputTransformerCalled = 0;
    all_filterCalled = 0;
    all_responderCalled = 0;
    all_outputTransformerCalled = 0;
    all_aggregatorCalled = 0;
    
    char dummyCtx[] = "Context";

    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockIn);
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockOut);
    
    msg_pipe_chain_t chain = {
        .splitter = all_splitter,
        .inputTransformer = all_inputTransformer,
        .filter = all_filter,
        .responder = all_responder,
        .outputTransformer = all_outputTransformer,
        .aggregator = all_aggregator,
    };
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = msg_core_createMessagingClient(settings),
        .out_chain = &chain,
        .user_context = dummyCtx,
    };

    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);
    
    const uint8_t data[] = {1,2,3,4,5,6,7,8};
    message_t message = {
        .data = &data,
        .length = 8,
    };
    
    msg_pipe_outboundSubscription(ctx->out, (void *) ctx, &message);
    
    TEST_ASSERT_EQUAL(1, all_splitterCalled);
    TEST_ASSERT_EQUAL(1, all_inputTransformerCalled);
    TEST_ASSERT_EQUAL(1, all_filterCalled);
    TEST_ASSERT_EQUAL(1, all_responderCalled);
    TEST_ASSERT_EQUAL(1, all_outputTransformerCalled);
    TEST_ASSERT_EQUAL(0, all_aggregatorCalled);    
}
void test_should_call_all_input_transformers()
{
    messagingSettings_t settings;
    messagingClient_t mockIn;
    messagingClient_t mockOut;   
    
    mockIn.start = startInStub;
    mockIn.connect = connectInStub;
    mockIn.subscribe = subscribeInStub;
    mockIn.loop = loopInStub;  
    mockIn.publish = splitter_two_messages_publish;  
    
    mockOut.start = startOutStub;
    mockOut.connect = connectOutStub;
    mockOut.subscribe = subscribeOutStub;
    mockOut.loop = loopOutStub;
    mockOut.publish = all_publish;

    all_splitterCalled = 0;
    all_inputTransformerCalled = 0;
    all_filterCalled = 0;
    all_responderCalled = 0;
    all_outputTransformerCalled = 0;
    all_aggregatorCalled = 0;
    
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockIn);
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockOut);
    
    char dummyCtx[] = "Context";

    msg_pipe_chain_t chain = {
        .splitter = all_splitter,
        .inputTransformer = all_inputTransformer,
        .filter = all_filter,
        .responder = all_responder,
        .outputTransformer = all_outputTransformer,
        .aggregator = all_aggregator,
    };
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = msg_core_createMessagingClient(settings),
        .in_chain = &chain,
        .user_context = dummyCtx,
    };

    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);
    
    const uint8_t data[] = {1,2,3,4,5,6,7,8};
    message_t message = {
        .data = &data,
        .length = 8,
    };
    
    msg_pipe_inboundSubscription(ctx->in, (void *) ctx, &message);
    
    TEST_ASSERT_EQUAL(1, all_splitterCalled);
    TEST_ASSERT_EQUAL(1, all_inputTransformerCalled);
    TEST_ASSERT_EQUAL(1, all_filterCalled);
    TEST_ASSERT_EQUAL(1, all_responderCalled);
    TEST_ASSERT_EQUAL(1, all_outputTransformerCalled);
    TEST_ASSERT_EQUAL(0, all_aggregatorCalled);   
}
static int dummyPreConnectHookCalled = 0;

void dummyPreConnectHook(msg_pipe_ctx_t * ctx)
{
    dummyPreConnectHookCalled ++;
}
void test_should_call_pre_in_connection_hook()
{
    messagingSettings_t settings;
    messagingClient_t mockIn;
    messagingClient_t mockOut;   
    
    mockIn.start = startInStub;
    mockIn.connect = connectInStub;
    mockIn.subscribe = subscribeInStub;
    mockIn.loop = loopInStub;  
    mockIn.publish = splitter_two_messages_publish;  
    
    mockOut.start = startOutStub;
    mockOut.connect = connectOutStub;
    mockOut.subscribe = subscribeOutStub;
    mockOut.loop = loopOutStub;
    mockOut.publish = all_publish;
    
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockIn);
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockOut);
    
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = msg_core_createMessagingClient(settings),
        .preInConnectHook = dummyPreConnectHook,
    };

    dummyPreConnectHookCalled = 0;
    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);
    
    TEST_ASSERT_EQUAL(1, dummyPreConnectHookCalled);
 }
 void test_should_call_pre_out_connection_hook()
{
    messagingSettings_t settings;
    messagingClient_t mockIn;
    messagingClient_t mockOut;   
    
    mockIn.start = startInStub;
    mockIn.connect = connectInStub;
    mockIn.subscribe = subscribeInStub;
    mockIn.loop = loopInStub;  
    mockIn.publish = splitter_two_messages_publish;  
    
    mockOut.start = startOutStub;
    mockOut.connect = connectOutStub;
    mockOut.subscribe = subscribeOutStub;
    mockOut.loop = loopOutStub;
    mockOut.publish = all_publish;
    
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockIn);
    msg_core_createMessagingClient_ExpectAndReturn(settings,&mockOut);
    
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = msg_core_createMessagingClient(settings),
        .preOutConnectHook = dummyPreConnectHook,
    };

    dummyPreConnectHookCalled = 0;
    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);
    
    TEST_ASSERT_EQUAL(1, dummyPreConnectHookCalled);
 } 
*/