#include "unity.h"

#include "msg_gcp_mqtt.h"

void setUp(void)
{
    
}

void test_bootstrap(void)
{
    gcpSettings_t settings = {
        .host = "1.1.1.1",
        .port = 8080
    };
    
    messagingClient_t * client = msg_gcp_createGcpClient(settings);
}