#include "messaging.h"
#include "unity.h"

static int outTimes = 0;
static int inTimes = 0;

void outHandler(message_t message) 
{
    outTimes ++;
    return;
}

void inHandler(message_t message) 
{
    inTimes ++;
    return;
}

void test_bootstrap(void)
{
    
    messageClient_t * client = NULL;
    
    messagingClientInit(&client);

    TEST_ASSERT_NOT_NULL(client);
}
void test_publish_returns_zero(void)
{
    
    message_t message;

    uint8_t data[] = {0x0,0x01};
    message.data = (data_t *) &data;
    message.length = 2;

    messageClient_t * client = NULL;
    
    messagingClientInit(&client);

    client->registerHandlers(client, inHandler, outHandler);

    int ret = client->publish(client, message);
    
    TEST_ASSERT_EQUAL(0,ret);
}
void test_publish_calls_outgoing_handler(void)
{
    
    message_t message;

    uint8_t data[] = {0x0,0x01};
    message.data = (data_t *) &data;
    message.length = 2;

    messageClient_t * client = NULL;
    
    messagingClientInit(&client);

    client->registerHandlers(client, inHandler, outHandler);

    client->publish(client, message);
    
    TEST_ASSERT_EQUAL(2,outTimes);
}
void test_register_handlers_returns_zero(void)
{
    messageClient_t * client = NULL;

    messagingClientInit(&client);

    int ret = client->registerHandlers(client, inHandler, outHandler);

    TEST_ASSERT_EQUAL(0,ret);
}
void test_register_handlers_can_be_called(void)
{
    messageClient_t * client = NULL;

    message_t message;

    uint8_t data[] = {0x0,0x01};
    message.data = (data_t *) &data;
    message.length = 2;
    
    messagingClientInit(&client);

    client->registerHandlers(client, inHandler, outHandler);

    client->incomingHandler(message);
    
    TEST_ASSERT_EQUAL(1,inTimes);
}

void outHandler_two(message_t message)
{
    const uint8_t data[] = {0x0,0x01};

    TEST_ASSERT_EQUAL_MEMORY(data, message.data,2);
}
void test_published_message_data(void)
{
    messageClient_t * client = NULL;

    message_t message;

    uint8_t data[] = {0x0,0x01};
    message.data = (data_t *) &data;
    message.length = 2;
    
    messagingClientInit(&client);

    client->registerHandlers(client, inHandler, outHandler_two);

    client->publish(client,message);
    
    TEST_ASSERT_EQUAL(1,inTimes);
}