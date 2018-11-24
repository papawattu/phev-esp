#ifndef _APP_H_
#define _APP_H_

#include "msg_mqtt.h"
#include "phev_controller.h"
#include "gcp_jwt.h"
#include "phev_store.h"

phevCtx_t * app_createPhevController(msg_mqtt_t mqtt, phevStore_t * store);
char * app_displayConfig(phevConfig_t * config);

#endif