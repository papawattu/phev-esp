#ifndef _JSON_BIN_H_
#define _JSON_BIN_H_

#ifndef TEST
#include "cJSON.h"
#endif
#include "msg_core.h" 


message_t * transformLightsJSONToBin(message_t *);

#endif