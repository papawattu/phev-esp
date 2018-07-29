#include "unity.h"
#include "mock_msg_core.h"
#include "mock_msg_pipe_splitter.h"
#include "msg_pipe.h"
void setUp() 
{

}
void tearDown()
{

}
static int loopInStubCalled = 0;
static int loopOutStubCalled = 0;

void loopInStub(messagingClient_t * ctx)
{
    loopInStubCalled ++;
}
void loopOutStub(messagingClient_t * ctx)
{
    loopOutStubCalled ++;
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