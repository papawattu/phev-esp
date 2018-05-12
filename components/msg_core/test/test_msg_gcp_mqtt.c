#include "unity.h"

#include "mock_msg_core.h"
#include "mock_msg_mqtt.h"
#include "msg_gcp_mqtt.h"

void setUp(void)
{
}

char * dummyJwt(void) 
{
    return NULL;
}
void test_createGcpClient(void)
{
    messagingClient_t msgClient;
    gcp_ctx_t ctx;
    ctx.host = "1.1.1.1";
    msgClient.ctx = &ctx;
    msg_core_createMessagingClient_IgnoreAndReturn(&msgClient);

    gcpSettings_t settings = {
        .host = "1.1.1.1",
        .port = 8080,
        .clientId = "client",
        .jwt = dummyJwt,
        .mqttClient = NULL
    };
    
    messagingClient_t * client = msg_gcp_createGcpClient(settings);
    TEST_ASSERT_NOT_NULL(client);
    TEST_ASSERT_EQUAL_STRING((const char *) "1.1.1.1", ((gcp_ctx_t *) client->ctx)->host);
}
//void test_createGcpClient(void)
//{

//}