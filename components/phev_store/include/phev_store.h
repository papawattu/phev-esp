#ifndef _PHEV_STORE_H_
#define _PHEV_STORE_H_

#include <stdint.h>
#include <stddef.h>

#include <stdbool.h>

#include "phev_config.h"

#define DEVICEID_SIZE 18 

typedef struct phevRegister_t
{
    void * handle;
    size_t length;
    uint8_t data[]; 
} phevRegister_t;

typedef struct phevStoreConnectionConfig_t 
{
    phevWifi_t wifi;
    char * host;
    uint16_t port;
    char * pppUser;
    char * pppPassword;
    char * pppAPN;
    char * mqttUri;
    char * gcpProjectId;
    char * gcpLocation;
    char * gcpRegistry;
} phevStoreConnectionConfig_t;

typedef struct phevStore_t
{
    unsigned int * handle;
    phevRegister_t * registers[256]; 
    phevStoreConnectionConfig_t * config;
    bool registered;
    bool configured;
    char deviceId[12];
} phevStore_t;

phevStore_t * phev_store_init(uint8_t *);
int phev_store_add(phevStore_t * store,uint8_t reg, uint8_t *data, size_t length);
phevRegister_t * phev_store_get(phevStore_t *,uint8_t);
int phev_store_compare(phevStore_t *store,uint8_t reg,uint8_t * data);
phevStoreConnectionConfig_t * phev_store_getConnectionConfig(phevStore_t * store);
void phev_store_storeConnectionConfig(phevStore_t * store, phevStoreConnectionConfig_t * config);


#endif