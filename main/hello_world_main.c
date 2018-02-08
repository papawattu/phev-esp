/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "esp_event_loop.h"

#include "nvs_flash.h"
#define MQTT_TASK 1
#include "MQTTClient.h"

const int CONNECTED_BIT = BIT0;

static const char *TAG = "OutlanderConnect";

static EventGroupHandle_t wifi_event_group;

struct {
  char* address;
  enum { clientid_maxlen = 256, clientid_size };
  char clientid[clientid_size];
  char* deviceid;
  char* ecpath;
  char* projectid;
  char* region;
  char* registryid;
  char* rootpath;
  char* topic;
  char* payload;
} opts = {
  "ssl://mqtt.googleapis.com:8883", /* Address */
  "projects/intense-wavelet-343/locations/us-central1/registries/mqtt-test-reg/devices/mqtt-c-rsq2",
  "mqtt-c-rsq2",
  "ec_private.pem",
  "intense-wavelet-343",
  "us-central1",
  "mqtt-test-reg",
  "roots.pem",
  "/devices/mqtt-c-rsq2/events",
  "Hello world!"
};

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        /* This is a workaround as ESP32 WiFi libs don't currently
           auto-reassociate. */
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}
static void initialise_wifi(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "BTHub3-HSZ3",
            .password = "simpsons",
        },
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}


void messageArrived(MessageData* data)
{
	printf("Message arrived on topic\n");
}

static void prvMQTTEchoTask(void)
{
	/* connect to m2m.eclipse.org, subscribe to a topic, send and receive messages regularly every 1 sec */
	MQTTClient client;
	Network network;
	unsigned char sendbuf[80], readbuf[80];
	int rc = 0, 
		count = 0;
	MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;

	NetworkInit(&network);
	MQTTClientInit(&client, &network, 30000, sendbuf, sizeof(sendbuf), readbuf, sizeof(readbuf));

	char* address = "iot.eclipse.org";
	if ((rc = NetworkConnect(&network, address, 1883)) != 0)
		printf("Return code from network connect is %d\n", rc);
/*
#if defined(MQTT_TASK)
	if ((rc = MQTTStartTask(&client)) != pdPASS)
		printf("Return code from start tasks is %d\n", rc);
#endif
*/
	connectData.MQTTVersion = 3;
	connectData.clientID.cstring = "FreeRTOS_sample";

	if ((rc = MQTTConnect(&client, &connectData)) != 0)
		printf("Return code from MQTT connect is %d\n", rc);
	else
		printf("MQTT Connected\n");

	if ((rc = MQTTSubscribe(&client, "FreeRTOS/sample/#", 2, messageArrived)) != 0)
		printf("Return code from MQTT subscribe is %d\n", rc);

	while (++count)
	{
		MQTTMessage message;
		char payload[30];

		message.qos = 1;
		message.retained = 0;
		message.payload = payload;
		sprintf(payload, "message number %d", count);
		message.payloadlen = strlen(payload);

		if ((rc = MQTTPublish(&client, "FreeRTOS/sample/a", &message)) != 0)
			printf("Return code from MQTT publish is %d\n", rc);
#if !defined(MQTT_TASK)
		if ((rc = MQTTYield(&client, 1000)) != 0)
			printf("Return code from yield is %d\n", rc);
#endif
	}

	/* do not return */
}

void vStartMQTTTasks(uint16_t usTaskStackSize, UBaseType_t uxTaskPriority)
{
	}

void app_main()
{
    ESP_ERROR_CHECK( nvs_flash_init() );
    initialise_wifi();
    BaseType_t x = 0L;

	xTaskCreate(prvMQTTEchoTask,	/* The function that implements the task. */
			"MQTTEcho0",			/* Just a text name for the task to aid debugging. */
			4096,	/* The stack size is defined in FreeRTOSIPConfig.h. */
			NULL,		/* The task parameter, not used in this case. */
			5,		/* The priority assigned to the task is defined in FreeRTOSConfig.h. */
			NULL);				/* The task handle is not used. */

}
