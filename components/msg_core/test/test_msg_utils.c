#include "unity.h"
#include "msg_utils.h"
#include "mock_logger.h"

void setUp(void)
{
    hexdump_Ignore();
}
void test_create_msg(void)
{
    uint8_t data[] = {1,2,3,4};
    message_t * message = msg_utils_createMsg(data,4);

    TEST_ASSERT_EQUAL(4, message->length);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(data, message->data,4);
}
void test_copy_message(void)
{
    uint8_t data[] = {1,2,3,4};
    message_t * message = msg_utils_createMsg(data,4);

    message_t * copied = msg_utils_copyMsg(message);

    TEST_ASSERT_EQUAL(4,copied->length);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(message->data, copied->data, 4);
}

void test_destroy_message(void)
{
    uint8_t data[] = {1,2,3,4};
    message_t * message = msg_utils_createMsg(data,4);

    msg_utils_destroyMsg(message);

}
void test_concat_messages(void)
{
    uint8_t data1[] = {1,2,3,4};
    message_t * message1 = msg_utils_createMsg(data1,4);
    uint8_t data2[] = {5,6,7,8,9};
    uint8_t data3[] = {1,2,3,4,5,6,7,8,9};
    message_t * message2 = msg_utils_createMsg(data2,5);

    message_t * message3 = msg_utils_concatMessages(message1,message2);

    TEST_ASSERT_NOT_NULL(message3);
    TEST_ASSERT_EQUAL(9,message3->length);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(&data3, message3->data,9);
}