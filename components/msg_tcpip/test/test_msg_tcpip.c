#include "unity.h"

#include "mock_msg_core.h"
#include "mock_sockets.h"
#include "msg_tcpip.h"


void setUp(void)
{
    
}
void test_tcpip_client_start(void)
{
    bzero_Ignore();
    htons_IgnoreAndReturn(0);
    inet_aton_Ignore();
    socket_ExpectAndReturn(0,0,0,0);
    connect_IgnoreAndReturn(0);   
    messagingSettings_t settings;
    settings.host = "1.2.3.4";
    settings.port = 1234;

    int ret = tcpIpClientStart(NULL,settings);

    TEST_ASSERT_EQUAL(0,ret);
} 
void test_create_tcpip_client(void)
{
    createMessagingClient_IgnoreAndReturn(1);
    tcpipsettings_t settings;
    settings.host = "1.2.3.4";

    settings.port = 1234;

    messagingClient_t * client = createTcpIpClient(settings);

    TEST_ASSERT_NOT_NULL(client);
} 