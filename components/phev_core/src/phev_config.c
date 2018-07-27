#include <stdio.h>
#include "phev_config.h"
#include "cJSON.h"

char * getConfigString(cJSON * json, char * option) 
{
    cJSON * value = cJSON_GetObjectItemCaseSensitive(json, option);
    
    return value->valuestring;
}
uint16_t getConfigInt(cJSON * json, char * option) 
{
    cJSON * value = cJSON_GetObjectItemCaseSensitive(json, option);
    
    return value->valueint;
}

long getConfigLong(cJSON * json, char * option) 
{
    cJSON * value = cJSON_GetObjectItemCaseSensitive(json, option);
    if(value == NULL) {
        return 0;
    }
    return (long) value->valuedouble;
}
bool getConfigBool(cJSON * json, char * option) 
{
    cJSON * value = cJSON_GetObjectItemCaseSensitive(json, option);
    if(value == NULL) {
        return false;
    }    
    return cJSON_IsTrue(value);
}

void phev_config_parseUpdateConfig(phevConfig_t * config, cJSON * update)
{
    config->updateConfig.latestBuild = getConfigLong(update, UPDATE_CONFIG_LATEST_BUILD);
}
phevConfig_t * phev_config_parseConfig(const char * config)
{
    phevConfig_t * phevConfig = malloc(sizeof(phevConfig_t));

    cJSON * json = cJSON_Parse((const char *) config);

    char * string = cJSON_Print(json);

    if(json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            printf("Error before: %s\n", error_ptr);
        }
        return NULL;
    } 
    cJSON * update = cJSON_GetObjectItemCaseSensitive(json, UPDATE_CONFIG_JSON);

    if(update != NULL)
    {
        phev_config_parseUpdateConfig(phevConfig, update);
    } else {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            printf("Error before: %s\n", error_ptr);
        }
        return NULL;
    }
    
    return phevConfig;
}