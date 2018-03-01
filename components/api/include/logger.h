#ifndef _LOGGER_H_
#define _LOGGER_H_

#ifdef TEST
void log_info(const char *msg) { }
void log_warn(const char *msg) { }
void log_error(const char *msg) { }
    
#else
#include "esp_log.h"

const char *APP_TAG = "APP";
void log_info(const char *msg) 
{ 
    ESP_LOGI(APP_TAG,"%s",msg);
}
void log_warn(const char *msg) 
{ 
    ESP_LOGW(APP_TAG,"%s",msg);
}
void log_error(const char *msg) 
{ 
    ESP_LOGE(APP_TAG,"%s",msg);
}

#endif
#endif