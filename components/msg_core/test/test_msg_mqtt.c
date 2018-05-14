#include "unity.h"

#include "mock_msg_core.h"
#include "msg_mqtt.h"

static int mock_init_Called = 0;
static int mock_start_Called = 0;
static int mock_publish_Called = 0;


handle_t mock_init(const config_t *config) 
{
    mock_init_Called ++;
    return 1;
}
err_t mock_start(handle_t client) 
{
    mock_start_Called++;
    return OK;
}

int mock_publish(msg_mqtt_t *mqtt,topic_t topic, message_t *message) 
{
    mock_publish_Called++;
    return 1234;
}

    

void test_start(void)
{
    msg_mqtt_t mqtt = {
        .init = mock_init,
        .start = mock_start
    };
    msg_mqtt_settings_t settings = {
        .host = "www.test.com",
        .port = 8883,
        .mqtt = &mqtt

    };
    handle_t handle = mqtt_start(&settings);

    TEST_ASSERT_EQUAL(1,mock_init_Called);
    TEST_ASSERT_EQUAL(1,mock_start_Called);
    
}

void test_publish(void)
{
    msg_mqtt_t mqtt = {
        .publish = mock_publish
    };

    message_t message = {
        .data = {1,2,3,4},
        .length = 4
    };

    int msgId = publish(&mqtt, "my topic", &message);

    TEST_ASSERT_EQUAL(1,mock_publish_Called);
    TEST_ASSERT_EQUAL(1234,msgId);
}