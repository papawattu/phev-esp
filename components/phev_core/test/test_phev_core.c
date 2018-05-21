#include "unity.h"
#include "phev_core.h"

const uint8_t singleMessage[] = {0x6f, 0x0a, 0x00, 0x12, 0x11, 0x05, 0x16, 0x15, 0x03, 0x0d, 0x01, 0xff};
const uint8_t doubleMessage[] = {0x6f, 0x0a, 0x00, 0x12, 0x11, 0x05, 0x16, 0x15, 0x03, 0x0d, 0x01, 0xff, 0x6f, 0x0a, 0x00, 0x12, 0x11, 0x05, 0x16, 0x15, 0x03, 0x0d, 0x01, 0xff};

void test_split_message_single_correct_size(void)
{
    phevMessage_t msg;

    int ret = phev_core_firstMessage(singleMessage, &msg);

    TEST_ASSERT_EQUAL(12, ret);
}

void test_split_message_single_correct_command(void)
{
    phevMessage_t msg;

    int ret = phev_core_firstMessage(singleMessage, &msg);

    TEST_ASSERT_EQUAL(0x6f, msg.command);
}
void test_split_message_single_correct_length(void)
{
    phevMessage_t msg;

    int ret = phev_core_firstMessage(singleMessage, &msg);

    TEST_ASSERT_EQUAL(0x0a, msg.length);
}
void test_split_message_single_correct_type(void)
{
    phevMessage_t msg;

    int ret = phev_core_firstMessage(singleMessage, &msg);

    TEST_ASSERT_EQUAL(REQUEST_TYPE, msg.type);
}
void test_split_message_single_correct_reg(void)
{
    phevMessage_t msg;

    int ret = phev_core_firstMessage(singleMessage, &msg);

    TEST_ASSERT_EQUAL(0x12, msg.reg);
}
void test_split_message_single_correct_data(void)
{
    phevMessage_t msg;
    uint8_t data[] = {0x11, 0x05, 0x16, 0x15, 0x03, 0x0d, 0x01}; 

    int ret = phev_core_firstMessage(singleMessage, &msg);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(data, msg.data, 7);
}
void test_split_message_double_correct(void)
{
    phevMessage_t msg;
    uint8_t data[] = {0x11, 0x05, 0x16, 0x15, 0x03, 0x0d, 0x01}; 

    int ret = phev_core_firstMessage(doubleMessage, &msg);

    ret = phev_core_firstMessage(doubleMessage + ret, &msg);

    TEST_ASSERT_EQUAL(0x12, msg.reg);    
}

void test_encode_message_single(void)
{
    uint8_t data[] = {0x11, 0x05, 0x16, 0x15, 0x03, 0x0d, 0x01}; 

    phevMessage_t msg = {
        .command = 0x6f,
        .length = 0x0a,
        .type = REQUEST_TYPE,
        .reg = 0x12,
        .data = &data,
        .checksum = 0xff,
    };
    
    uint8_t * out = phev_core_encodeMessage(&msg);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(singleMessage, out, 12);    
}