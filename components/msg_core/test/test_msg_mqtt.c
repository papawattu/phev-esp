#include "unity.h"

#include "mock_msg_core.h"
#include "msg_utils.h"
#include "msg_mqtt.h"


#define TOPIC "topic"
#define MSG_ID 1234
#define DATA_LEN 4

uint8_t DATA[] = {1,2,3,4};

static int mock_init_Called = 0;
static int mock_start_Called = 0;
static int mock_publish_Called = 0;


handle_t mock_init(const msg_mqtt_config_t *config) 
{
    mock_init_Called ++;
    msg_mqtt_t * mqtt = config->user_context;
    return NULL;
}
msg_mqtt_err_t mock_start(handle_t client) 
{
    mock_start_Called++;
    return OK;
}

int mock_publish(handle_t client, const char *topic, const char *data, int len, int qos, int retain)
{
    mock_publish_Called++;

    TEST_ASSERT_EQUAL_STRING(TOPIC,topic);
    TEST_ASSERT_EQUAL(4, len);
    //TEST_ASSERT_EQUAL_HEX8_ARRAY(&DATA,data,4);
    
    return MSG_ID;
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
        .publish = &mock_publish
    };

    message_t * message = malloc(sizeof(message_t));

    message->data = malloc(4);
    message->length = 4;

    memcpy(message->data, &DATA, 4);
    message_t * out = malloc(sizeof(message_t));
    out->data = malloc(sizeof(DATA));
    memcpy(out->data,DATA,sizeof(DATA));
    out->length = sizeof(DATA);
    
    //msg_utils_copyMsg_ExpectAndReturn(&message,out);
    
    int msgId = msg_mqtt_publish(&mqtt, TOPIC, message);

    TEST_ASSERT_EQUAL(1,mock_publish_Called);
    TEST_ASSERT_EQUAL(MSG_ID,msgId);
}

static int mock_incoming_Called = 0;

messagingClient_t * msg_client = NULL;

void mock_incoming(messagingClient_t * client, message_t * message) 
{
    mock_incoming_Called ++;

    TEST_ASSERT_EQUAL(DATA_LEN, message->length);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(DATA, message->data,DATA_LEN);
    TEST_ASSERT_EQUAL(msg_client, client);

}

void test_event_data(void)
{

    messagingClient_t client = {

    };

    msg_client = &client;

    msg_mqtt_t mqtt = {
        .incoming_cb = &mock_incoming,
        .client = msg_client
    };

    
    mqtt_event_t event = {
        .event_id = MSG_MQTT_EVENT_DATA,
        .data = DATA,
        .data_len = DATA_LEN,
        .topic = TOPIC,
        .topic_len = 5,
        .user_context = &mqtt
    };

    dataEvent(&event);

    TEST_ASSERT_EQUAL(1,mock_incoming_Called);

}