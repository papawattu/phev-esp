#include "unity.h"

#include "mock_messaging.h"
#include "mock_sockets.h"
#include "socket_client.h"


void setUp(void)
{
    bzero_Ignore();
    htons_IgnoreAndReturn(0);
    inet_aton_Ignore();
    socket_ExpectAndReturn(0,0,0,0);
    connect_IgnoreAndReturn(0);
    
}
void test_socket_client_start(void)
{
    
    messagingSettings_t settings;
    settings.host = "1.2.3.4";
    settings.port = 1234;

    int ret = socketClientStart(settings);

    TEST_ASSERT_EQUAL(0,ret);
}
/*
void test_socket_client_start(void)
{
    
    messagingSettings_t settings;
    settings.host = "1.2.3.4";
    settings.port = 1234;

    int ret = socketClientStart(settings);

    TEST_ASSERT_EQUAL(0,ret);
} */