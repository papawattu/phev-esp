#include "unity.h"
#include "msg_core.h"
#include "msg_utils.h"

static int outTimes = 0;
static int inTimes = 0;
static int started = 0;
static int connected = 0;

uint8_t * copyData(uint8_t  * data, size_t length) 
{
    uint8_t * out = malloc(length);
    memcpy(out,data,length);
    return out;
}

void setUp(void)
{

}
void outHandler(messagingClient_t *client, message_t *message) 
{
    outTimes ++;
    return;
}

message_t * inHandler(messagingClient_t *client) 
{
    message_t * message = malloc(sizeof(message_t));
    inTimes ++;
    return message;
}

void test_bootstrap(void)
{
    
    messagingClient_t * client = NULL;
    
    msg_core_messagingClientInit(&client);

    TEST_ASSERT_NOT_NULL(client);
} 
void test_publish_returns_zero(void)
{
    
    message_t * message = malloc(sizeof(message_t));

    uint8_t data[] = {0x0,0x01};

    message->data =malloc(2);
    message->length = 2;

    messagingClient_t * client = NULL;
    
    msg_core_messagingClientInit(&client);

    msg_core_registerHandlers(client, inHandler, outHandler);

    int ret = client->publish(client, message);
    
    TEST_ASSERT_EQUAL(0,ret);
} 
void test_publish_calls_outgoing_handler(void)
{
    
//    uint8_t data[] = {0x0,0x01};

    message_t * message = malloc(sizeof(message_t));

    uint8_t * data = malloc(2); 

    message->data = (data_t *) data;
    
    message->length = 2;

    messagingClient_t * client = NULL;
    
    msg_core_messagingClientInit(&client);

    msg_core_registerHandlers(client, inHandler, outHandler);

    client->publish(client, message);
    
    TEST_ASSERT_EQUAL(2,outTimes);
} 
void test_register_handlers_returns_zero(void)
{
    messagingClient_t * client = NULL;

    msg_core_messagingClientInit(&client);

    int ret = msg_core_registerHandlers(client, inHandler, outHandler);

    TEST_ASSERT_EQUAL(0,ret);
}  
void test_register_handlers_can_be_called(void)
{
    messagingClient_t * client = NULL;

    msg_core_messagingClientInit(&client);

    msg_core_registerHandlers(client, inHandler, outHandler);

    client->incomingHandler(client);
    
    TEST_ASSERT_EQUAL(1,inTimes);
}
//TODO Fixme
void outHandler_two(messagingClient_t *client, message_t *message)
{
    const uint8_t data[] = {0x0,0x01};
    TEST_ASSERT_EQUAL_MEMORY(data, message->data,2);
} 
void test_published_message_data(void)
{
    messagingClient_t * client = NULL;

    message_t * message = malloc(sizeof(message_t));

    uint8_t data[] = {0x0,0x01};
    message->data = copyData(data,sizeof(data));
    message->length = 2;
    
    msg_core_messagingClientInit(&client);

    msg_core_registerHandlers(client, inHandler, outHandler_two);

    client->publish(client,message);
    
    TEST_ASSERT_EQUAL(1,inTimes);
}

void test_create_messaging_client()
{
    messagingSettings_t settings;
    messagingClient_t * client = msg_core_createMessagingClient(settings);

    TEST_ASSERT_NOT_NULL(client);
}

int mock_start(messagingClient_t *client)
{
    started = 1;
    return 0;
} 
void test_create_messaging_client_start()
{
    messagingSettings_t settings;
    settings.start = mock_start;
    messagingClient_t * client = msg_core_createMessagingClient(settings);

    TEST_ASSERT_EQUAL(mock_start,client->start);
}
int mock_stop(messagingClient_t *client)
{
    started = 0;
    return 0;
} 
void test_create_messaging_client_stop()
{
    messagingSettings_t settings;
    settings.stop = mock_stop;
    messagingClient_t * client = msg_core_createMessagingClient(settings);

    TEST_ASSERT_EQUAL(mock_stop,client->stop);
}
int mock_connect(messagingClient_t *client)
{
    connected = 1;
    return 0;
} 
void test_create_messaging_client_connect()
{
    messagingSettings_t settings;
    settings.connect = mock_connect;
    messagingClient_t * client = msg_core_createMessagingClient(settings);

    TEST_ASSERT_EQUAL(mock_connect,client->connect);
} 
void test_messaging_client_start()
{
    messagingSettings_t settings;
    settings.start = mock_start;
    messagingClient_t * client = msg_core_createMessagingClient(settings);

    started = 0;

    client->start(client);

    TEST_ASSERT_EQUAL(1,started);
} 
void test_messaging_client_stop()
{
    messagingSettings_t settings;
    settings.stop = mock_stop;
    messagingClient_t * client = msg_core_createMessagingClient(settings);

    started = 1;
    
    client->stop(client);

    TEST_ASSERT_EQUAL(0,started);
} 
void test_messaging_client_connect()
{
    messagingSettings_t settings;
    settings.connect = mock_connect;
    messagingClient_t * client = msg_core_createMessagingClient(settings);

    connected = 0;
    
    client->connect(client);

    TEST_ASSERT_EQUAL(1,connected);
}
static int mock_pub_called = 0;
static int mock_sub_called = 0;
message_t *latest_message;

void mock_pub(messagingClient_t *client, message_t *message) 
{
    mock_pub_called ++;
    latest_message = message;
}
message_t * mock_sub(messagingClient_t *client)
{
    mock_sub_called ++;
    return latest_message;
}
static int subscriptionCalled = 0;

void subscription(messagingClient_t *client, void * params, message_t *message)
{
    subscriptionCalled ++;    
    TEST_ASSERT_NOT_NULL(client);
    TEST_ASSERT_NOT_NULL(message);
    
    uint8_t data[] = {0x0,0x01};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(data,message->data,2);
    TEST_ASSERT_EQUAL(2,message->length);
}
/*
void test_messaging_pub_sub()
{
    messagingSettings_t settings;
    message_t * message = malloc(sizeof(message_t));
    uint8_t data2[] = {0x0,0x01};
    message->data = malloc(2);
    memcpy(message->data, &data2,2);
    
    message->length = 2;
    settings.start = mock_start;
    settings.connect = mock_connect;
    settings.outgoingHandler = mock_pub;
    settings.incomingHandler = mock_sub;
    
    messagingClient_t * client = msg_core_createMessagingClient(settings);
    
    client->start(client);
    client->connect(client);
    client->publish(client, message);
    client->subscribe(client, NULL, subscription);

    client->loop(client);

    TEST_ASSERT_EQUAL(1, mock_pub_called);
    TEST_ASSERT_EQUAL(1, mock_sub_called);
    TEST_ASSERT_EQUAL(1, subscriptionCalled);
} 
*/