#ifndef _LOGGER_H_
#define _LOGGER_H_
#include <stdint.h>
#include "msg_core.h"

void msgBundleDump(const char * tag, messageBundle_t * bundle);

#ifdef __linux__

void hexdump(char * tag, unsigned char * buffer, int length, int level);

#define LOG_DEBUG 0
#define LOG_I(TAG, FORMAT , ...) printf ("INFO - " FORMAT "\n", ##__VA_ARGS__)
#define LOG_V(TAG, FORMAT , ...) printf ("VERBOSE - " FORMAT "\n", ##__VA_ARGS__)
#define LOG_D(TAG, FORMAT , ...) printf ("DEBUG - " FORMAT "\n", ##__VA_ARGS__)
#define LOG_E(TAG, FORMAT , ...) printf ("ERROR - " FORMAT "\n", ##__VA_ARGS__)
#define LOG_BUFFER_HEXDUMP(TAG, BUFFER, LENGTH, LEVEL) hexdump(TAG, BUFFER, LENGTH, LEVEL)
#define LOG_MSG_BUNDLE(TAG, BUNDLE) msgBundleDump(TAG, BUNDLE)
#else
#include "esp_log.h"

#define LOG_DEBUG ESP_LOG_DEBUG
#define LOG_I(...) ESP_LOGI(__VA_ARGS__)
#define LOG_V(...) ESP_LOGV(__VA_ARGS__)
#define LOG_D(...) ESP_LOGD(__VA_ARGS__)
#define LOG_E(...) ESP_LOGE(__VA_ARGS__)
#define LOG_BUFFER_HEXDUMP(...) ESP_LOG_BUFFER_HEXDUMP(__VA_ARGS__)
#define LOG_MSG_BUNDLE(TAG, BUNDLE) msgBundleDump(TAG, BUNDLE)
#endif
#endif
