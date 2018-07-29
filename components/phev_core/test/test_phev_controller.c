#include "unity.h"
#include "msg_core.h"
#include "msg_tcpip.h"
#include "mock_msg_pipe.h"
#include "phev_controller.h"
#include "mock_phev_core.h"
#include "mock_msg_utils.h"
#include "mock_phev_config.h"

//#ifdef ESP_PLATFORM
//#include "cJSON.h"
//#else
#include <cjson/cJSON.h>
//#endif


void test_handle_event(void)
{
    phevEvent_t event = {
        .type = CONNECT_REQUEST,
    };

    TEST_ASSERT_EQUAL(PHEV_OK, phev_controller_handleEvent(&event));
}

void test_phev_controller_init(void)
{
    messagingClient_t inClient;
    messagingClient_t outClient;

    msg_pipe_ctx_t pipe;

    msg_pipe_IgnoreAndReturn(&pipe);
    
    phevSettings_t settings = {
        .in     = &inClient,
        .out    = &outClient,
    };

    phevCtx_t * ctx = phev_controller_init(&settings);

    TEST_ASSERT_NOT_NULL(ctx);
    TEST_ASSERT_EQUAL(&pipe, ctx->pipe);
    TEST_ASSERT_EQUAL(0, ctx->queueSize);
} 

void test_phev_controller_init_set_phev_ctx(void)
{
    messagingClient_t inClient;
    messagingClient_t outClient;

    void * dummyCtx;

    msg_pipe_ctx_t pipe = {
        .user_context = dummyCtx,
    };

    msg_pipe_IgnoreAndReturn(&pipe);
    
    phevSettings_t settings = {
        .in     = &inClient,
        .out    = &outClient,
    };

    phevCtx_t * ctx = phev_controller_init(&settings);

    TEST_ASSERT_NOT_NULL(ctx);
    TEST_ASSERT_EQUAL(dummyCtx, ctx->pipe->user_context);
} 
static int fake_publish_called = 0;
void fake_publish(void * ctx, message_t * message)
{
    
    uint8_t data[] = {1,2,3};

    TEST_ASSERT_EQUAL(3, message->length);
    TEST_ASSERT_EQUAL_MEMORY(data,message->data,3);
    fake_publish_called ++;
}
void test_phev_controller_send_message(void)
{
    messagingClient_t outClient = {
        .publish = fake_publish,
    };

    
    msg_pipe_ctx_t pipe = {
        .out = &outClient,
    };

    msg_pipe_IgnoreAndReturn(&pipe);
    
    phevSettings_t settings = {
        .out    = &outClient,
    };

    uint8_t data[] = {1,2,3};

    message_t * message = msg_utils_createMsg(data,sizeof(data));

    phevCtx_t * ctx = phev_controller_init(&settings);

    phev_controller_sendMessage(ctx,  message);

    TEST_ASSERT_EQUAL(1, fake_publish_called);
} 
void test_phev_controller_initConfig(void)
{

}
void test_phev_controller_initState(void)
{
    phevState_t state;

    phev_controller_initState(&state);

    TEST_ASSERT_EQUAL(0, state.connectedClients);
}

void test_phev_controller_config_splitter_connected(void)
{
    const char * msg_data = "{ \"state\": { \"connectedClients\": 1 } }";
    
    message_t * message = malloc(sizeof(message_t));
    
    message->data = msg_data;
    message->length = sizeof(msg_data);
    
    message_t * start = malloc(sizeof(message_t));
    
    start->data = "START";
    start->length = sizeof("START");
    
    phevConfig_t config = {

    };

    phev_config_parseConfig_IgnoreAndReturn(&config);
    phev_config_checkForConnection_IgnoreAndReturn(true);
    phev_config_checkForHeadLightsOn_IgnoreAndReturn(false);
    phev_core_startMessageEncoded_IgnoreAndReturn(start);
    messageBundle_t * out = phev_controller_configSplitter(NULL, message);
    
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(1,out->numMessages);
    
    TEST_ASSERT_EQUAL_STRING(start->data,out->messages[0]->data);
}
void test_phev_controller_config_splitter_not_connected(void)
{
    const char * msg_data = "{ \"state\": { \"connectedClients\": 0 } }";
    
    message_t * message = malloc(sizeof(message_t));
    
    message->data = msg_data;
    message->length = sizeof(msg_data);
    
    message_t * start = malloc(sizeof(message_t));
    
    start->data = "START";
    start->length = sizeof("START");
    
    phevConfig_t config = {

    };

    phev_config_parseConfig_IgnoreAndReturn(&config);
    phev_config_checkForConnection_IgnoreAndReturn(false);
    phev_core_startMessageEncoded_IgnoreAndReturn(start);
    phev_config_checkForHeadLightsOn_IgnoreAndReturn(false);
    messageBundle_t * out = phev_controller_configSplitter(NULL, message);
    
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(0,out->numMessages);
}
void test_phev_controller_config_splitter_headLightsOn(void)
{
    const char * msg_data = "";
    
    message_t * message = malloc(sizeof(message_t));
    
    message->data = msg_data;
    message->length = sizeof(msg_data);
    
    const uint8_t lightsOn[] = {0xf6,0x04,0x00,0x0a,0x02,0xff};
    phevConfig_t config = {

    };
    message_t outMsg = {
        .data = lightsOn,
        .length = 6,
    };
    phev_config_parseConfig_IgnoreAndReturn(&config);
    phev_config_checkForConnection_IgnoreAndReturn(false);
    phev_core_startMessageEncoded_IgnoreAndReturn(NULL);
    phev_config_checkForHeadLightsOn_IgnoreAndReturn(true);
    phev_core_simpleRequestCommandMessage_IgnoreAndReturn(NULL);
    phev_core_convertToMessage_IgnoreAndReturn(&outMsg);
    phev_core_destroyMessage_Ignore();
    messageBundle_t * out = phev_controller_configSplitter(NULL, message);
    
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(1,out->numMessages);
    TEST_ASSERT_NOT_NULL(out->messages[0]);
    TEST_ASSERT_EQUAL(6,out->messages[0]->length);
    
    TEST_ASSERT_NOT_NULL(out->messages[0]->data);
    
    TEST_ASSERT_EQUAL_MEMORY(lightsOn, out->messages[0]->data,6);
}