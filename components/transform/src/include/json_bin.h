#ifndef _JSON_BIN_H_
#define _JSON_BIN_H_

#ifdef ESP_PLATFORM
#include "cJSON.h"
#else
#include <cjson/cJSON.h>
#endif
#include "msg_core.h" 


message_t * transformLightsJSONToBin(message_t *);

#endif