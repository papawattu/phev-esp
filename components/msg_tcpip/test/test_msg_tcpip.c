#include "unity.h"

#include "mock_msg_core.h"
#include "mock_sockets.h"
#include "msg_tcpip.h"


void setUp(void)
{
    
}
void test_create_tcpip_client(void)
{
    createMessagingClient_IgnoreAndReturn(1);
    tcpIpSettings_t settings;
    settings.host = "1.2.3.4";

    settings.port = 1234;

    messagingClient_t * client = createTcpIpClient(settings);

    TEST_ASSERT_NOT_NULL(client);
} 
void test_tcpip_client_connect(void)
{
    tcpIpSettings_t settings;
    settings.host = "1.2.3.4";
    settings.port = 1234;
    messagingClient_t client;
    client.ctx = (void *) &settings;
    createMessagingClient_IgnoreAndReturn(&client);
    bzero_Ignore();
    htons_IgnoreAndReturn(0);
    inet_aton_Ignore();
    socket_ExpectAndReturn(0,0,0,0);
    connect_IgnoreAndReturn(0);   
    
    TEST_ASSERT_EQUAL(0,tcpipConnect(&client));

    TEST_ASSERT_EQUAL(1,client.connected);
} 
void test_tcpip_client_outgoing_handler(void)
{
    tcpip_ctx_t ctx;
    uint8_t buffer[4];
    ctx.host = "1.2.3.4";
    ctx.port = 1234;
    ctx.socket = 1;
    ctx.readBuffer = &buffer;
    messagingClient_t client;
    client.ctx = (void *) &ctx;
    message_t message;
    uint8_t data[] = {1,2,3,4}; 
    message.data = &data;
    message.length = 4;
    write_ExpectAndReturn(1,&data,4,0);
    tcpipOutgoingHandler(&client,&message);

} 
void test_tcpip_client_incoming_handler_no_message(void)
{
    tcpip_ctx_t ctx;
    uint8_t * buffer = malloc(TCPIP_CLIENT_READ_BUF_SIZE);
    ctx.socket = 1;
    ctx.readBuffer = buffer;
    messagingClient_t client;
    client.ctx = (void *) &ctx;
    read_IgnoreAndReturn(0);
    message_t *message = tcpipIncomingHandler(&client);
    TEST_ASSERT_NULL(message);
} 
void test_tcpip_client_incoming_handler(void)
{
    tcpip_ctx_t ctx;
    uint8_t buffer[] = {1,2,3,4};
    ctx.socket = 1;
    ctx.readBuffer = (uint8_t *) &buffer;
    messagingClient_t client;
    client.ctx = (void *) &ctx;
    read_ExpectAndReturn(1,&buffer,TCPIP_CLIENT_READ_BUF_SIZE,4);
    message_t *message = tcpipIncomingHandler(&client);
    TEST_ASSERT_NOT_NULL(message);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(buffer,message->data,4);
    TEST_ASSERT_EQUAL(4,message->length);
} 