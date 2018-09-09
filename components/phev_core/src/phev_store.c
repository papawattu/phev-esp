#include "phev_store.h"
#include "logger.h"

const static char *APP_TAG = "PHEV_STORE";

phevStore_t * phev_store_create(void)
{
    LOG_V(APP_TAG,"START - create");

    phevStore_t * store = malloc(sizeof(phevStore_t));

    LOG_V(APP_TAG,"END - create");

    return store;
}
int phev_store_add(phevStore_t * store,uint8_t reg, uint8_t *data, size_t length)
{
    LOG_V(APP_TAG,"START - add");

    phevRegister_t * out = malloc(sizeof(phevRegister_t) + length);
    
    out->length = length;
    memcpy(out->data,data,length);
    
    if(store->registers[reg] != NULL)
    {
        free(store->registers[reg]);
    }
    
    store->registers[reg] = out;
    
    LOG_V(APP_TAG,"END - add");

    return 1;
}
phevRegister_t * phev_store_get(phevStore_t * store,uint8_t reg)
{
    LOG_V(APP_TAG,"START - get");

    phevRegister_t * message = store->registers[reg];

    LOG_V(APP_TAG,"END - create");

    return message;
}
int phev_store_compare(phevStore_t *store,uint8_t reg,uint8_t * data)
{
    if(store->registers[reg] == NULL) return -1;
    
    return memcmp(data,store->registers[reg]->data,store->registers[reg]->length);
}
