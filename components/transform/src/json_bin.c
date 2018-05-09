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
/*
message_t *transformLightsJSONToBin(message_t *message)
{
    message_t *bin = malloc(sizeof(message_t));
    data_t *data = malloc(SIMPLE_MESSAGE_SIZE);

    cJSON *json = cJSON_Parse((const char *)message->data);

    cJSON *lights = NULL;

    lights = cJSON_GetObjectItemCaseSensitive(json, LIGHTS_CMD);

    if(lights == NULL) 
    {
        return NULL;
    }
    if (strcmp(ON, lights->valuestring) == 0)
    {
        memcpy(data, (unsigned char[]){0xf6, 0x04, 0x00, LIGHTS_REG, LIGHTS_ON_VAL, 0x05}, 6);
    }
    else
    {
        memcpy(data, (unsigned char[]){0xf6, 0x04, 0x00, LIGHTS_REG, LIGHTS_OFF_VAL, 0x05}, 6);
    }

    bin->data = data;
    bin->length = SIMPLE_MESSAGE_SIZE;

    return bin;
} */