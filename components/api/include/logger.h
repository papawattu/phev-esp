#ifndef _APP_LOGGER_H_
#define _APP_LOGGER_H_

#ifdef TEST
void log_info(const char *msg) { }
void log_warn(const char *msg) { }
void log_error(const char *msg) { }
    
#else
#include "esp_log.h"

#ifndef _BOB_
#define _BOB_
const char *MAIN_TAG = "APP";
void log_info(const char *msg, ...) 
{ 
    ESP_LOGI(MAIN_TAG,"%s",msg);
}
void log_warn(const char *msg, ...) 
{ 
    ESP_LOGW(MAIN_TAG,"%s",msg);
}
void log_error(const char *msg, ...) 
{ 
    ESP_LOGE(MAIN_TAG,"%s",msg);
}
#endif
#endif
#endif