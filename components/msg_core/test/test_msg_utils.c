#include "unity.h"
#include "msg_utils.h"


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