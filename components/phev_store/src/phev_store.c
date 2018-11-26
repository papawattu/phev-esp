#include "phev_store.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "logger.h"
#include <string.h>

const static char *APP_TAG = "PHEV_STORE";

phevStore_t * phev_store_init(uint8_t * mac)
{
    LOG_V(APP_TAG,"START - init");

    phevStore_t * store = malloc(sizeof(phevStore_t));

    store->config = NULL;
    store->registered = false;

    asprintf(&store->deviceId, "device%02x%02x%02x%02x%02x%02x",(unsigned char) mac[0], (unsigned char) mac[1],(unsigned char) mac[2], (unsigned char) mac[3], (unsigned char) mac[4], (unsigned char) mac[5]);
    
    LOG_I(APP_TAG,"Device ID %s",store->deviceId);
    
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
    
    uint8_t configured;

    err = nvs_open("phev_store", NVS_READWRITE, &store->handle);
    
    if (err != ESP_OK) {
        LOG_E(APP_TAG,"Error (%s) opening NVS handle!", esp_err_to_name(err));
    } else {
        err = nvs_get_u8(store->handle, "config_set", &configured);
        
        if(err == ESP_OK && configured == 1) 
        {
            LOG_I(APP_TAG,"Found config");
        
            phev_store_getConnectionConfig(store); 
        } else {
            if(err == ESP_ERR_NVS_NOT_FOUND)
            {
                LOG_I(APP_TAG,"Not found config");
                store->config = NULL;
            }
        }
    //nvs_close(my_handle);
    
    }

    LOG_V(APP_TAG,"END - init");

    return store;
}

void phev_store_storeConnectionConfig(phevStore_t * store, phevStoreConnectionConfig_t * config)
{
    esp_err_t err;

    err = nvs_set_str(store->handle,"email",store->email);
    if(err != ESP_OK)
    {
        LOG_E(APP_TAG,"NVS set error %s",esp_err_to_name(err));
    }    
    err = nvs_set_str(store->handle,"wifi_ssid",config->wifi.ssid);
    if(err != ESP_OK)
    {
        LOG_E(APP_TAG,"NVS set error %s",esp_err_to_name(err));
    }
    
    err = nvs_set_str(store->handle,"wifi_password",config->wifi.password);
    if(err != ESP_OK)
    {
        LOG_E(APP_TAG,"NVS set error %s",esp_err_to_name(err));
    }
    
    err = nvs_set_str(store->handle,"host",config->host);
    if(err != ESP_OK)
    {
        LOG_E(APP_TAG,"NVS set error %s",esp_err_to_name(err));
    }
    
    err = nvs_set_u16(store->handle,"port",config->port);
    if(err != ESP_OK)
    {
        LOG_E(APP_TAG,"NVS set error %s",esp_err_to_name(err));
    }
    
    err = nvs_set_str(store->handle,"ppp_user",config->pppUser);
    if(err != ESP_OK)
    {
        LOG_E(APP_TAG,"NVS set error %s",esp_err_to_name(err));
    }
    
    err = nvs_set_str(store->handle,"ppp_password",config->pppPassword);
    if(err != ESP_OK)
    {
        LOG_E(APP_TAG,"NVS set error %s",esp_err_to_name(err));
    }
    
    err = nvs_set_str(store->handle,"ppp_apn",config->pppAPN);
    if(err != ESP_OK)
    {
        LOG_E(APP_TAG,"NVS set error %s",esp_err_to_name(err));
    }

    err = nvs_set_u8(store->handle,"config_set",1);
    if(err != ESP_OK)
    {
        LOG_E(APP_TAG,"NVS set error %s",esp_err_to_name(err));
    }

    err = nvs_commit(store->handle); 

    
    if(err != ESP_OK)
    {
        LOG_E(APP_TAG,"NVS commit error %s",esp_err_to_name(err));
    }
    if(store->config != NULL) 
    {
        free(store->config);
    }
    store->config = config;

    store->configured = true;
}
phevStoreConnectionConfig_t * phev_store_getConnectionConfig(phevStore_t * store)
{
    phevStoreConnectionConfig_t * config = malloc(sizeof(phevStoreConnectionConfig_t));
    
    store->configured = false;
    
    size_t size;
    esp_err_t err; 
    
    
    err = nvs_get_str(store->handle,"email",NULL, &size);
    if(err != ESP_OK)
    {
        LOG_E(APP_TAG,"NVS get size error %s",esp_err_to_name(err));
        return NULL;
    }
    store->email = malloc(size);
    err = nvs_get_str(store->handle,"email",store->email, &size);
    if(err != ESP_OK)
    {
        LOG_E(APP_TAG,"NVS get error %s",esp_err_to_name(err));
        return NULL;
    }
    char * ssid = malloc(MAX_WIFI_SSID_LEN + 1);
    err = nvs_get_str(store->handle,"wifi_ssid",ssid, &size);
    if(err != ESP_OK)
    {
        LOG_E(APP_TAG,"NVS get error %s",esp_err_to_name(err));
        return NULL;
    }
    strncpy(&config->wifi.ssid,ssid,size);
    char * password = malloc(MAX_WIFI_PASSWORD_LEN +1);
    
    err = nvs_get_str(store->handle,"wifi_password",password, &size);
    if(err != ESP_OK)
    {
        LOG_E(APP_TAG,"NVS get error %s",esp_err_to_name(err));
        return NULL;
    }
    strncpy(&config->wifi.password,password,size);
    
    err = nvs_get_str(store->handle,"host",NULL, &size);
    if(err != ESP_OK)
    {
        LOG_E(APP_TAG,"NVS get size error %s",esp_err_to_name(err));
        return NULL;
    }
    config->host = malloc(size);
    err = nvs_get_str(store->handle,"host",config->host, &size);
    if(err != ESP_OK)
    {
        LOG_E(APP_TAG,"NVS get error %s",esp_err_to_name(err));
        return NULL;
    }
    err = nvs_get_u16(store->handle,"port",&config->port);
    if(err != ESP_OK)
    {
        LOG_E(APP_TAG,"NVS get error %s",esp_err_to_name(err));
        return NULL;
    }
    err = nvs_get_str(store->handle,"ppp_user",NULL, &size);
    if(err != ESP_OK)
    {
        LOG_E(APP_TAG,"NVS get size error %s",esp_err_to_name(err));
        return NULL;
    }
    config->pppUser = malloc(size);
    
    err = nvs_get_str(store->handle,"ppp_user",config->pppUser, &size);
    if(err != ESP_OK)
    {
        LOG_E(APP_TAG,"NVS get size error %s",esp_err_to_name(err));
        return NULL;
    }
    err = nvs_get_str(store->handle,"ppp_password",NULL, &size);
    if(err != ESP_OK)
    {
        LOG_E(APP_TAG,"NVS get size error %s",esp_err_to_name(err));
        return NULL;
    }
    config->pppPassword = malloc(size);
    err = nvs_get_str(store->handle,"ppp_password",config->pppPassword, &size);
    if(err != ESP_OK)
    {
        LOG_E(APP_TAG,"NVS get error %s",esp_err_to_name(err));
        return NULL;
    }
    err = nvs_get_str(store->handle,"ppp_apn",NULL, &size);
    if(err != ESP_OK)
    {
        LOG_E(APP_TAG,"NVS get size error %s",esp_err_to_name(err));
        return NULL;
    }
    config->pppAPN = malloc(size);
    err = nvs_get_str(store->handle,"ppp_apn",config->pppAPN, &size);
    if(err != ESP_OK)
    {
        LOG_E(APP_TAG,"NVS get error %s",esp_err_to_name(err));
        return NULL;
    }

    ESP_LOGI(APP_TAG, "Car details");
    
    ESP_LOGI(APP_TAG, "Host %s",config->host);
    ESP_LOGI(APP_TAG, "Port %u",config->port);
    ESP_LOGI(APP_TAG, "SSID %s",config->wifi.ssid);
    ESP_LOGI(APP_TAG, "Password %s",config->wifi.password);

    ESP_LOGI(APP_TAG, "PPP / GSM details");

    ESP_LOGI(APP_TAG, "PPP User %s",config->pppUser);
    ESP_LOGI(APP_TAG, "PPP Password %s",config->pppPassword);
    ESP_LOGI(APP_TAG, "PPP APN %s",config->pppAPN);

    if(store->config != NULL) 
    {
        free(store->config);
    }
    store->config = config;
    store->configured = true;
    return config;  
    
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
