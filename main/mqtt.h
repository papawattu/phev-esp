#ifndef _MQTT_H_

#define _MQTT_H_

#include "freertos/FreeRTOS.h"
#include "esp_log.h"

static const char *TAG = "OutlanderConnect";

typedef struct Timer 
{
	TickType_t xTicksToWait;
	TimeOut_t xTimeOut;
    struct timeval end_time;
} Timer;

typedef struct Network Network;

struct Network
{
	int my_socket;
	int (*mqttread) (Network*, unsigned char*, int, int);
	int (*mqttwrite) (Network*, unsigned char*, int, int);
	void (*disconnect) (Network*);
};

void TimerInit(Timer* timer) {
	ESP_LOGD(TAG, "TimerInit");
	timer->end_time = (struct timeval){0, 0};
}

char TimerIsExpired(Timer* timer) {
	struct timeval now, res;
	gettimeofday(&now, NULL);
	timersub(&timer->end_time, &now, &res);
	ESP_LOGD(TAG, "TimerIsExpired %ld.%ld",res.tv_sec,res.tv_usec);
	ESP_LOGD(TAG, "TimerIsExpired %d",res.tv_sec < 0 || (res.tv_sec == 0 && res.tv_usec <= 0));
	return res.tv_sec < 0 || (res.tv_sec == 0 && res.tv_usec <= 0);
}


void TimerCountdownMS(Timer* timer, unsigned int timeout) {
	struct timeval now;
	ESP_LOGD(TAG, "TimerCountdownMS");
	gettimeofday(&now, NULL);
	struct timeval interval = {timeout / 1000, (timeout % 1000) * 1000};
	timeradd(&now, &interval, &timer->end_time);
}


void TimerCountdown(Timer* timer, unsigned int timeout) {
	ESP_LOGD(TAG, "TimerCountdown");
	struct timeval now;
	gettimeofday(&now, NULL);
	struct timeval interval = {timeout, 0};
	timeradd(&now, &interval, &timer->end_time);
}


int TimerLeftMS(Timer* timer) {
	ESP_LOGD(TAG, "TimerLeftMS");
	struct timeval now, res;
	gettimeofday(&now, NULL);
	timersub(&timer->end_time, &now, &res);
	return (res.tv_sec < 0) ? 0 : res.tv_sec * 1000 + res.tv_usec / 1000;
}
typedef struct Mutex
{
	SemaphoreHandle_t sem;
} Mutex;

void MutexInit(Mutex* m)
{

}
int MutexLock(Mutex* m) 
{

}
int MutexUnlock(Mutex* m)
{

}

typedef struct Thread
{
	TaskHandle_t task;
} Thread;

int ThreadStart(Thread* thread, void (*fn)(void*), void* arg)
{
    return 0;
}

int FreeRTOS_read(Network* network, unsigned char* buffer, int size, int x)
{
    return 0;
}
int FreeRTOS_write(Network* network, unsigned char* buffer, int size, int x)
{
    return 0;
}
void FreeRTOS_disconnect(Network* network)
{
    
}

void NetworkInit(Network* network)
{

}
int NetworkConnect(Network* network, char* x, int y)
{
    return 0;
}

#endif