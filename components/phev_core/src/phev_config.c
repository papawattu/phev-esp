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

// This has moved to phev_config
void phev_config_setUpdateConfig(phevUpdateConfig_t * config, const char * ssid, 
                                        const char * password,
                                        const char * host,
                                        const char * path,
                                        uint16_t port,
                                        int build,
                                        bool updateOverPPP,
                                        bool forceUpdate
                                        )
{
    strcpy(config->updateWifi.ssid,ssid);
    config->updateWifi.ssid[strlen(ssid)] = '\0';
    
    strcpy(config->updateWifi.password,password);

    config->updateWifi.password[strlen(password)] = '\0';
    
    config->updateHost = malloc(strlen(host));
    strcpy(config->updateHost,host);    
    
    config->updatePath = malloc(strlen(path));
    strcpy(config->updatePath,path);
    
    const char * buildPath = NULL;     
    asprintf(&buildPath,"%s%s%010d.bin", config->updatePath,IMAGE_PREFIX,build);
    
    config->updateImageFullPath = buildPath;

    config->updatePort = port;

    config->latestBuild = build;

    config->updateOverPPP = updateOverPPP;

    config->forceUpdate = forceUpdate;
}

void phev_config_parseUpdateConfig(phevConfig_t * config, cJSON * update)
{
    long build = getConfigLong(update,UPDATE_CONFIG_LATEST_BUILD);

    phev_config_setUpdateConfig(&config->updateConfig, getConfigString(update,UPDATE_CONFIG_SSID), 
                                        getConfigString(update,UPDATE_CONFIG_PASSWORD),
                                        getConfigString(update,UPDATE_CONFIG_HOST),
                                        getConfigString(update,UPDATE_CONFIG_PATH),
                                        getConfigInt(update,UPDATE_CONFIG_PORT),
                                        build,
                                        getConfigBool(update, UPDATE_CONFIG_OVER_GSM),
                                        getConfigBool(update, UPDATE_CONFIG_FORCE_UPDATE)
                                    );
                                
    
}
void phev_config_parseConnectionConfig(phevConfig_t * config, cJSON * connection)
{
    config->connectionConfig.host = getConfigString(connection, CONNECTION_CONFIG_HOST);
    config->connectionConfig.port = getConfigInt(connection, CONNECTION_CONFIG_PORT);

    strcpy(config->connectionConfig.carConnectionWifi.ssid, getConfigString(connection, CONNECTION_CONFIG_SSID)); 
    strcpy(config->connectionConfig.carConnectionWifi.password, getConfigString(connection, CONNECTION_CONFIG_PASSWORD)); 
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

    cJSON * connection = cJSON_GetObjectItemCaseSensitive(json, CONNECTION_CONFIG_JSON);

    if(connection)
    {
        phev_config_parseConnectionConfig(phevConfig, connection);
    }

    return phevConfig;
}