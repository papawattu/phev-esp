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
void test_should_publish_message_incoming()
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
    
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = msg_core_createMessagingClient(settings),
    };

    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);

    ctx->loop(ctx);
    

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
    
    inputTransformCalled = 0;
        
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = msg_core_createMessagingClient(settings),
        .in_inputTransformer = inputTransform,
    };

    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);
    
    TEST_ASSERT_EQUAL(inputTransform,ctx->in_inputTransformer);
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
        
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = msg_core_createMessagingClient(settings),
        .out_inputTransformer = inputTransform,
    };

    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);
    
    TEST_ASSERT_EQUAL(inputTransform,ctx->out_inputTransformer);
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
    
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = msg_core_createMessagingClient(settings),
        .out_outputTransformer = outputTransform,
    };

    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);
    TEST_ASSERT_EQUAL(outputTransform,ctx->out_outputTransformer);
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
    
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = msg_core_createMessagingClient(settings),
        .in_outputTransformer = outputTransform,
    };

    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);
    TEST_ASSERT_EQUAL(outputTransform,ctx->in_outputTransformer);
} 
static int splitterCalled = 0;
void splitter(message_t * message, void (* next)(message_t *))
{
    splitterCalled ++;
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
    
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = msg_core_createMessagingClient(settings),
        .in_splitter = splitter,
    };

    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);
    TEST_ASSERT_EQUAL(splitter,ctx->in_splitter);
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
    
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = msg_core_createMessagingClient(settings),
        .out_splitter = splitter,
    };

    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);
    TEST_ASSERT_EQUAL(splitter,ctx->out_splitter);
}  
void test_should_call_out_splitter()
{
    TEST_IGNORE("Not yet implemented");
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
    
    msg_pipe_settings_t pipe_settings = {
        .in = msg_core_createMessagingClient(settings),
        .out = msg_core_createMessagingClient(settings),
        .out_splitter = splitter,
    };

    msg_pipe_ctx_t * ctx = msg_pipe(pipe_settings);
    
    //msg_pipe_outboundSubscription(ctxclient, void * params, message_t * messagectx->out->in
    TEST_ASSERT_EQUAL(1,splitterCalled);
}