#ifndef _PHEV_STORE_H_
#define _PHEV_STORE_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "phev_core.h"

typedef struct phevRegister_t
{
    size_t length;
    uint8_t data[]; 
} phevRegister_t;

typedef struct phevStore_t
{
    phevRegister_t * registers[256]; 
} phevStore_t;

phevStore_t * phev_store_create(void);
int phev_store_add(phevStore_t * store,uint8_t reg, uint8_t *data, size_t length);
phevRegister_t * phev_store_get(phevStore_t *,uint8_t);
int phev_store_compare(phevStore_t *store,uint8_t reg,uint8_t * data);


#endif