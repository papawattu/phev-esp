#include "unity.h"
#include "msg_pipe_splitter.h"
#include "mock_msg_core.h"
#include "mock_msg_pipe.h"

static int splitterSingleCalled = 0;
message_t * splitter_single_message(message_t * message)
{
    splitterSingleCalled ++;
    return NULL;
}

static int mock_single_splitter_called = 0;

messageBundle_t * mock_single_splitter(void * ctx, message_t *message) 
{
    messageBundle_t * out = malloc(sizeof(messageBundle_t));

    out->messages[0] = message;

    out->numMessages = 1;

    mock_single_splitter_called ++;
    
    return out;
} 
void test_should_split_single()
{
    messagingClient_t * client = NULL;
    msg_pipe_ctx_t * ctx = NULL;
    msg_pipe_chain_t chain = {
        .splitter = mock_single_splitter,
    };

    const uint8_t data[] = {1,2,3,4};

    message_t * message = malloc(sizeof(message_t));
    message->data = malloc(4);
    memcpy(message->data, data, 4);
    message->length = 4;
    
    messageBundle_t * out = msg_pipe_splitter(ctx, client, &chain, message);
    
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(1,out->numMessages);
    TEST_ASSERT_NOT_NULL(out->messages);
    TEST_ASSERT_NOT_NULL(out->messages[0]);
    TEST_ASSERT_EQUAL(1,mock_single_splitter_called);
    TEST_ASSERT_NOT_NULL(out->messages);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(data,out->messages[0]->data,4);
}
static int mock_double_splitter_called = 0;

messageBundle_t * mock_double_splitter(void * ctx, message_t *message) 
{
    const uint8_t first_data[] = {1,2,3,4};
    const uint8_t second_data[] = {5,6,7,8};

    message_t * first_message = malloc(sizeof(message_t));
    first_message->data = malloc(sizeof(first_data));
    memcpy(first_message->data, first_data, sizeof(first_data));
    message->length = sizeof(first_data);
    
    message_t * second_message = malloc(sizeof(message_t));
    second_message->data = malloc(sizeof(second_data));
    memcpy(second_message->data, second_data, sizeof(second_data));
    message->length = sizeof(second_data);
    
    messageBundle_t * out = malloc(sizeof(messageBundle_t));

    out->messages[0] = first_message;
    out->messages[1] = second_message;

    out->numMessages = 2;

    mock_double_splitter_called ++;
    
    return out;
} 
void test_should_split_double()
{
    messagingClient_t * client = NULL;
    msg_pipe_ctx_t * ctx = NULL;
    msg_pipe_chain_t chain = {
        .splitter = mock_double_splitter,
    };

    const uint8_t data[] = {1,2,3,4,5,6,7,8};
    const uint8_t first_data[] = {1,2,3,4};
    const uint8_t second_data[] = {5,6,7,8};


    message_t * message = malloc(sizeof(message_t));
    message->data = malloc(sizeof(data));
    memcpy(message->data, data, sizeof(data));
    message->length = sizeof(data);
    
    messageBundle_t * out = msg_pipe_splitter(ctx, client, &chain, message);
    
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(2,out->numMessages);
    TEST_ASSERT_NOT_NULL(out->messages);
    TEST_ASSERT_NOT_NULL(out->messages[0]);
    TEST_ASSERT_NOT_NULL(out->messages[1]);
    TEST_ASSERT_EQUAL(1,mock_double_splitter_called);
    TEST_ASSERT_NOT_NULL(out->messages);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(first_data,out->messages[0]->data,sizeof(first_data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(second_data,out->messages[1]->data,sizeof(second_data));

} 
