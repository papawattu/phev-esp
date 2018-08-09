#ifndef _LOGGER_H_
#define _LOGGER_H_

#ifdef __linux__

#define LOG_DEBUG 0
#define LOG_I(TAG, FORMAT , ...) printf ("INFO - " FORMAT "\n", ##__VA_ARGS__)
#define LOG_V(TAG, FORMAT , ...) printf ("VERBOSE - " FORMAT "\n", ##__VA_ARGS__)
#define LOG_D(TAG, FORMAT , ...) printf ("DEBUG - " FORMAT "\n", ##__VA_ARGS__)
#define LOG_E(TAG, FORMAT , ...) printf ("ERROR - " FORMAT "\n", ##__VA_ARGS__)
#define LOG_BUFFER_HEXDUMP(...) printf("Buffer hexdump is not impl in linux yet")
#else
#include "esp_log.h"

#define LOG_DEBUG ESP_LOG_DEBUG
#define LOG_I(...) ESP_LOGI(__VA_ARGS__)
#define LOG_V(...) ESP_LOGV(__VA_ARGS__)
#define LOG_D(...) ESP_LOGD(__VA_ARGS__)
#define LOG_E(...) ESP_LOGE(__VA_ARGS__)
#define LOG_BUFFER_HEXDUMP(...) ESP_LOG_BUFFER_HEXDUMP(__VA_ARGS__)

#endif
#endif
