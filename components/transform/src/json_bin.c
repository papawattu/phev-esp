#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json_bin.h"

#define LIGHTS_REG 0x0a
#define LIGHTS_ON_VAL 0x01
#define LIGHTS_OFF_VAL 0x02
#define LIGHTS_CMD "lights"
#define ON "on"
#define OFF "off"


message_t * transformLightsJSONToBin(message_t * message)
{
    message_t * bin = malloc(sizeof(message_t));
    int state = 0;

    cJSON *json = cJSON_Parse((const char *) message->data);

    cJSON *lights = NULL;

    lights = cJSON_GetObjectItemCaseSensitive(json,LIGHTS_CMD);

    if(strcmp(ON,lights->valuestring) == 0)
    {
        state = LIGHTS_ON_VAL;
    } else {
        state = LIGHTS_OFF_VAL;
    }

    unsigned char * data = malloc(6);
    
    memcpy(data,(unsigned char[]) {0xf6,0x04,0x00,LIGHTS_REG,state,0x05},6);

    bin->data = data;
    bin->length = 6;

    return bin;
}