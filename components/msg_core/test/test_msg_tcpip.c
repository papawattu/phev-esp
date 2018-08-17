#include "unity.h"
#include "mock_msg_core.h"
#include "msg_tcpip.h"
#include "mock_logger.h"
static int dummyReadNoMessageCalled = 0;
static int dummyReadCalled = 0;
static int dummyWriteCalled = 0;
static int dummyConnectCalled = 0;

int dummyRead_noMessage(int socket,uint8_t* buf, size_t size)
{
    dummyReadNoMessageCalled ++;
    return 0;
}
int dummyRead(int socket,uint8_t* buf, size_t size)
{
    //uint8_t * readBuffer = malloc(TCPIP_CLIENT_READ_BUF_SIZE);
    dummyReadCalled ++;
    return size;
}

int dummyWrite(int socket,uint8_t* buf, size_t size)
{
    dummyWriteCalled ++;
    return size;
}

int dummyConnect(messagingClient_t *client)
{
    dummyConnectCalled ++;
    return 1;
}

void setUp(void)
{
    hexdump_Ignore();
}
void test_create_tcpip_client(void)
{
    messagingClient_t ret;
    msg_core_createMessagingClient_IgnoreAndReturn(&ret);
    tcpIpSettings_t settings;
    
    messagingClient_t * client = msg_tcpip_createTcpIpClient(settings);

    TEST_ASSERT_NOT_NULL(client);
} 
void test_tcpip_client_connect(void)
{
    tcpip_ctx_t ctx = {
        .connect = dummyConnect,
    };
    messagingClient_t client;
    client.ctx = (void *) &ctx;
    msg_core_createMessagingClient_IgnoreAndReturn(&client);
    
    TEST_ASSERT_EQUAL(0,msg_tcpip_connect(&client));

    TEST_ASSERT_EQUAL(1,client.connected);
}  
void test_tcpip_client_outgoing_handler(void)
{
    uint8_t buffer[4];
    
    tcpip_ctx_t ctx = {
        .socket = 1,
        .readBuffer = &buffer,
        .write = dummyWrite,
        .read = dummyRead 
    };
    messagingClient_t client;
    client.ctx = (void *) &ctx;
    client.connected = 1;
    message_t message;
    uint8_t data[] = {1,2,3,4}; 
    message.data = &data;
    message.length = 4;
    msg_tcpip_outgoingHandler(&client,&message);
    TEST_ASSERT_EQUAL(1,dummyWriteCalled);
} 
void test_tcpip_client_incoming_handler_no_message(void)
{
    tcpip_ctx_t ctx = {
        .socket = 1,
        .readBuffer = malloc(TCPIP_CLIENT_READ_BUF_SIZE),
        .write = dummyWrite,
        .read = dummyRead_noMessage,
    };
    
    messagingClient_t client = {
        .ctx = &ctx,
        .connected = 1,
    };
    message_t *message = msg_tcpip_incomingHandler(&client);
    TEST_ASSERT_NULL(message);
    TEST_ASSERT_EQUAL(1,dummyReadNoMessageCalled);

} 
void test_tcpip_client_incoming_handler(void)
{
    tcpip_ctx_t ctx = {
        .socket = 1,
        .readBuffer = malloc(TCPIP_CLIENT_READ_BUF_SIZE),
        .write = dummyWrite,
        .read = dummyRead
    };
    messagingClient_t client = {
        .ctx = &ctx,
        .connected = 1,
    };
    client.ctx = (void *) &ctx;
    message_t *message = msg_tcpip_incomingHandler(&client);
    TEST_ASSERT_EQUAL(1,dummyReadCalled);
} 