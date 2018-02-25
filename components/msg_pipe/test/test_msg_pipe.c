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

int startInStub()
{
    startInStubNum ++;
}
static int connectInStubNum = 0;

int connectInStub()
{
    connectInStubNum ++;
}
static int startOutStubNum = 0;

int startOutStub()
{
    startOutStubNum ++;
}
static int connectOutStubNum = 0;

int connectOutStub()
{
    connectOutStubNum ++;
}

void test_should_call_start_incoming()
{
    messagingSettings_t settings;
    messagingClient_t mockIn;
    messagingClient_t mockOut;

    mockIn.start = startInStub;
    mockIn.connect = connectInStub;
    
    mockOut.start = startOutStub;
    mockOut.connect = connectOutStub;
    
    createMessagingClient_ExpectAndReturn(settings,&mockIn);
    createMessagingClient_ExpectAndReturn(settings,&mockOut);
    
    messagingClient_t *in = createMessagingClient(settings);
    messagingClient_t *out = createMessagingClient(settings);

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
    
    mockOut.start = startOutStub;
    mockOut.connect = connectOutStub;
    
    createMessagingClient_ExpectAndReturn(settings,&mockIn);
    createMessagingClient_ExpectAndReturn(settings,&mockOut);
    
    messagingClient_t *in = createMessagingClient(settings);
    messagingClient_t *out = createMessagingClient(settings);

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
    
    mockOut.start = startOutStub;
    mockOut.connect = connectOutStub;
    
    createMessagingClient_ExpectAndReturn(settings,&mockIn);
    createMessagingClient_ExpectAndReturn(settings,&mockOut);
    
    messagingClient_t *in = createMessagingClient(settings);
    messagingClient_t *out = createMessagingClient(settings);

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
    
    mockOut.start = startOutStub;
    mockOut.connect = connectOutStub;
    
    createMessagingClient_ExpectAndReturn(settings,&mockIn);
    createMessagingClient_ExpectAndReturn(settings,&mockOut);
    
    messagingClient_t *in = createMessagingClient(settings);
    messagingClient_t *out = createMessagingClient(settings);

    connectOutStubNum = 0;
        
    msg_pipe(in, out);

    TEST_ASSERT_EQUAL(1,connectOutStubNum);
} 