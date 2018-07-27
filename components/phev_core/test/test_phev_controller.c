#include "unity.h"
#include "msg_core.h"
#include "msg_tcpip.h"
#include "mock_msg_pipe.h"
#include "phev_controller.h"
#include "mock_phev_core.h"
#include "mock_msg_utils.h"
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
void test_phev_controller_car_connection_config(void)
{
    char host[32];

    tcpip_ctx_t tcpip_ctx;

    messagingClient_t inClient = {
        .ctx = &tcpip_ctx,
    };
    messagingClient_t outClient;

    void * dummyCtx;

    msg_pipe_ctx_t pipe = {
        .in = &inClient,
        .user_context = dummyCtx,
    };

    msg_pipe_IgnoreAndReturn(&pipe);
    
    phevSettings_t settings = {
        .in     = &inClient,
        .out    = &outClient,
    };

    phevCtx_t * ctx = phev_controller_init(&settings);

    phev_controller_setCarConnectionConfig(ctx,"abc","123","127.0.0.1",8080);    

    TEST_ASSERT_NOT_NULL(ctx->config);
    TEST_ASSERT_EQUAL_STRING("abc", ctx->config->carConnectionWifi.ssid);
    TEST_ASSERT_EQUAL_STRING("123", ctx->config->carConnectionWifi.password);
    TEST_ASSERT_EQUAL_STRING("127.0.0.1", ctx->config->host);
    TEST_ASSERT_EQUAL(8080, ctx->config->port);

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
} /*
void test_phev_controller_config_splitter(void)
{
    const char * config = "{"
	"\"connection\" : { " 
    "    \"clients\": 1"
    "}"
	"\"latestBuild\": 1531523469,"
	"\"update\": {"
	"	\"overGsm\": true,"
	"	\"ssid\": \"BTHub6-P535\","
	"	\"password\": \"S1mpsons\","
	"	\"host\": \"storage.googleapis.com\","
	"	\"port\": 80,"
	"	\"path\": \"/espimages/develop/\""
	"},"
	"\"carConnection\": {"
	"	\"ssid\": \"REMOTE45cfsc\","
	"	\"password\": \"fhcm852767\","
	"	\"host\": \"192.168.8.46\","
	"	\"port\": 8080"
	"},"
	"\"status\": {"
	"	\"headLightsOn\": true,"
	"	\"airConOn\": false"
	"}"
    "}"; 
    message_t * message = msg_utils_createMsg(config, sizeof(config));
    message_t * split = phev_controller_configSplitter(ctx, message);
} */

