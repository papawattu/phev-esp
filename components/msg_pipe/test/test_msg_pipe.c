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
    
    messagingClient_t *in = msg_core_createMessagingClient(settings);
    messagingClient_t *out = msg_core_createMessagingClient(settings);

    startInStubNum = 0;
        
    msg_pipe(in, out);
    
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
    
    messagingClient_t *in = msg_core_createMessagingClient(settings);
    messagingClient_t *out = msg_core_createMessagingClient(settings);

    connectInStubNum = 0;
        
    msg_pipe(in, out);

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
    
    messagingClient_t *in = msg_core_createMessagingClient(settings);
    messagingClient_t *out = msg_core_createMessagingClient(settings);

    startOutStubNum = 0;
        
    msg_pipe(in, out);

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
    
    messagingClient_t *in = msg_core_createMessagingClient(settings);
    messagingClient_t *out = msg_core_createMessagingClient(settings);

    connectOutStubNum = 0;
        
    msg_pipe(in, out);

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
    
    messagingClient_t *in = msg_core_createMessagingClient(settings);
    messagingClient_t *out = msg_core_createMessagingClient(settings);

    connectOutStubNum = 0;
        
    msg_pipe_ctx_t * ctx = msg_pipe(in, out);

    TEST_ASSERT_EQUAL(in,ctx->in);
    TEST_ASSERT_EQUAL(out,ctx->out);
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
    
    messagingClient_t *in = msg_core_createMessagingClient(settings);
    messagingClient_t *out = msg_core_createMessagingClient(settings);

    connectOutStubNum = 0;
        
    msg_pipe_ctx_t * ctx = msg_pipe(in, out);

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
    
    messagingClient_t *in = msg_core_createMessagingClient(settings);
    messagingClient_t *out = msg_core_createMessagingClient(settings);

    subscribeInStubNum = 0;
    subscribeOutStubNum = 0;
    
    msg_pipe_ctx_t * ctx = msg_pipe(in, out);

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
    
    messagingClient_t *in = msg_core_createMessagingClient(settings);
    messagingClient_t *out = msg_core_createMessagingClient(settings);

    msg_pipe_ctx_t * ctx = msg_pipe(in, out);

    ctx->loop(ctx);
    

} 
static int inputTransformCalled = 0;
message_t * inputTransform(message_t * message) 
{
    inputTransformCalled ++;
    return message;
}
void test_should_set_input_transformer()
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
    
    messagingClient_t *in = msg_core_createMessagingClient(settings);
    messagingClient_t *out = msg_core_createMessagingClient(settings);

    inputTransformCalled = 0;
        
    msg_pipe_ctx_t * ctx = msg_pipe(in, out);

    msg_pipe_transformer_t transformer = {
        .input = inputTransform,
        .output = NULL
    };

    msg_pipe_add_transformer(ctx, &transformer);

    TEST_ASSERT_EQUAL(1,ctx->numTransformers);
    TEST_ASSERT_EQUAL(inputTransform,ctx->transformers[0]->input);
}
static int outputTransformCalled = 0;
message_t * outputTransform(message_t * message) 
{
    outputTransformCalled ++;
    return message;
}
void test_should_set_output_transformer()
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
    
    messagingClient_t *in = msg_core_createMessagingClient(settings);
    messagingClient_t *out = msg_core_createMessagingClient(settings);

    outputTransformCalled = 0;
        
    msg_pipe_ctx_t * ctx = msg_pipe(in, out);

    msg_pipe_transformer_t transformer = {
        .input = NULL,
        .output = outputTransform
    };

    msg_pipe_add_transformer(ctx, &transformer);

    TEST_ASSERT_EQUAL(1,ctx->numTransformers);
    TEST_ASSERT_EQUAL(outputTransform,ctx->transformers[0]->output);
}
void test_should_set_multiple_transformers()
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
    
    messagingClient_t *in = msg_core_createMessagingClient(settings);
    messagingClient_t *out = msg_core_createMessagingClient(settings);

    outputTransformCalled = 0;
        
    msg_pipe_ctx_t * ctx = msg_pipe(in, out);

    msg_pipe_transformer_t transformer1 = {
        .input = NULL,
        .output = NULL
    };

    msg_pipe_transformer_t transformer2 = {
        .input = NULL,
        .output = NULL
    };
    msg_pipe_add_transformer(ctx, &transformer1);
    msg_pipe_add_transformer(ctx, &transformer2);

    TEST_ASSERT_EQUAL(2,ctx->numTransformers);
    TEST_ASSERT_EQUAL(&transformer1,ctx->transformers[0]);
    TEST_ASSERT_EQUAL(&transformer2,ctx->transformers[1]);
}