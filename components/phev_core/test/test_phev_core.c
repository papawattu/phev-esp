#include "unity.h"
#include "phev_core.h"

const uint8_t singleMessage[] = {0x6f, 0x0a, 0x00, 0x12, 0x11, 0x05, 0x16, 0x15, 0x03, 0x0d, 0x01, 0xdd};
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
        .checksum = 0xdd,
    };

    uint8_t * out;
    int num = phev_core_encodeMessage(&msg, &out);
    TEST_ASSERT_EQUAL(12, num);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(singleMessage, out, num);
}
void test_encode_message_single_checksum(void)
{
    uint8_t data[] = {0x00};

    phevMessage_t msg = {
        .command = 0x6f,
        .length = 0x04,
        .type = RESPONSE_TYPE,
        .reg = 0xaa,
        .data = &data,
    };

    uint8_t * out;
    int num = phev_core_encodeMessage(&msg, &out);
    TEST_ASSERT_EQUAL(0x1e, out[5]);
}
void test_simple_command_request_message(void)
{
    phevMessage_t *msg = phev_core_simpleRequestCommandMessage(0x01, 0xff);

    TEST_ASSERT_EQUAL(0xf6, msg->command);
    TEST_ASSERT_EQUAL(0x4, msg->length);
    TEST_ASSERT_EQUAL(REQUEST_TYPE, msg->type);
    TEST_ASSERT_EQUAL(0x1, msg->reg);
    TEST_ASSERT_EQUAL(0xff, msg->data[0]);
}
void test_simple_command_response_message(void)
{
    phevMessage_t *msg = phev_core_simpleResponseCommandMessage(0x01, 0xff);

    TEST_ASSERT_EQUAL(0xf6, msg->command);
    TEST_ASSERT_EQUAL(0x4, msg->length);
    TEST_ASSERT_EQUAL(RESPONSE_TYPE, msg->type);
    TEST_ASSERT_EQUAL(0x1, msg->reg);
    TEST_ASSERT_EQUAL(0xff, msg->data[0]);
}
void test_command_message(void)
{
    const uint8_t data[] = {0, 1, 2, 3, 4, 5};

    phevMessage_t *msg = phev_core_commandMessage(0x10, &data, sizeof(data));

    TEST_ASSERT_EQUAL(0xf6, msg->command);
    TEST_ASSERT_EQUAL(0x9, msg->length);
    TEST_ASSERT_EQUAL(REQUEST_TYPE, msg->type);
    TEST_ASSERT_EQUAL(0x10, msg->reg);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(&data, msg->data, sizeof(data));
}
void test_ack_message(void)
{
    const uint8_t reg = 0x10;
    const phevMessage_t *msg = phev_core_ackMessage(reg);

    TEST_ASSERT_EQUAL(0xf6, msg->command);
    TEST_ASSERT_EQUAL(0x4, msg->length);
    TEST_ASSERT_EQUAL(RESPONSE_TYPE, msg->type);
    TEST_ASSERT_EQUAL(0x10, msg->reg);
    TEST_ASSERT_EQUAL(0x00, *msg->data);
}
void test_start_message(void)
{
    const uint8_t mac[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
    const phevMessage_t *msg = phev_core_startMessage(&mac);

    TEST_ASSERT_EQUAL(START_SEND, msg->command);
    TEST_ASSERT_EQUAL(0x0a, msg->length);
    TEST_ASSERT_EQUAL(REQUEST_TYPE, msg->type);
    TEST_ASSERT_EQUAL(0x02, msg->reg);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(&mac, msg->data, sizeof(mac));
}
void test_ping_message(void)
{
    const uint8_t num = 1;

    const phevMessage_t * msg = phev_core_pingMessage(num);

    TEST_ASSERT_EQUAL(PING_SEND_CMD, msg->command);
    TEST_ASSERT_EQUAL(0x04, msg->length);
    TEST_ASSERT_EQUAL(REQUEST_TYPE, msg->type);
    TEST_ASSERT_EQUAL(num, msg->reg);
    TEST_ASSERT_EQUAL(0, *msg->data);
}
void test_response_handler_start(void)
{
    const uint8_t value = 0;
    phevMessage_t request = {
        .command = RESP_CMD,
        .length = 4,
        .type = REQUEST_TYPE,
        .reg = 0x29,
        .data = &value,  
    };
    phevMessage_t * msg = phev_core_responseHandler(&request);

    TEST_ASSERT_EQUAL(SEND_CMD, msg->command);
    TEST_ASSERT_EQUAL(0x04, msg->length);
    TEST_ASSERT_EQUAL(RESPONSE_TYPE, msg->type);
    TEST_ASSERT_EQUAL(0x29, msg->reg);
    TEST_ASSERT_EQUAL(0, *msg->data);   
}
void test_calc_checksum(void)
{
    const uint8_t data[] = {0x2f,0x04,0x00,0x01,0x01,0x00};
    uint8_t checksum = phev_core_checksum(data);
    TEST_ASSERT_EQUAL(0x35,checksum);
}