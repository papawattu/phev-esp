#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json_bin.h"

#define LIGHTS_REG      0x0a
#define LIGHTS_ON_VAL   0x01
#define LIGHTS_OFF_VAL  0x02
#define LIGHTS_CMD      "lights"
#define ON              "on"
#define OFF             "off"

char * getConfigString(cJSON * json, char * option) 
{
    cJSON * value = cJSON_GetObjectItemCaseSensitive(json, option);
    if(value == NULL) {
        //ESP_LOGE(APP_TAG,"Cannot find option %s", option);
        return NULL;
    }

    //ESP_LOGI(APP_TAG,"Option %s set to %s", option, value->valuestring);
    
    return value->valuestring;
}