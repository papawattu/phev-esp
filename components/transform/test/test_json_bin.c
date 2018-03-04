#include "unity.h"
#include "json_bin.h"


void test_bootstrap(void)
{
    
    message_t message = {
        .data = "{ \"lights\" : \"on\" }",
        .length = 4
    };

    message_t * out = transform(&message);
    TEST_ASSERT_NOT_NULL(out);
}
void test_lights_on(void)
{
    const unsigned char data[] = {0xf6,0x04,0x00,0x0a,0x01,0x05};

    message_t message = {
        .data = "{ \"lights\" : \"on\" }",
        .length = sizeof(message.data)
    };

    message_t * out = transform(&message);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(data,out->data,6);
}
void test_lights_off(void)
{
    const unsigned char data[] = {0xf6,0x04,0x00,0x0a,0x02,0x05};

    message_t message = {
        .data = "{ \"lights\" : \"off\" }",
        .length = sizeof(message.data)
    };

    message_t * out = transform(&message);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(data,out->data,6);
}
