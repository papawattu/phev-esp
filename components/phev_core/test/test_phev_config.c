#include "unity.h"
#include "phev_config.h"
#include "stdio.h"
#include <sys/stat.h>
#include "cJSON.h"
#define FILENAME "components/phev_core/test/config.json"

FILE * configFile;
char * buffer;

void setUp(void)
{
    configFile = fopen(FILENAME,"r");
    if (configFile == NULL) {
        TEST_FAIL_MESSAGE("Cannot open file");
    }
    
    struct stat st;
    stat(FILENAME, &st);
    size_t size = st.st_size;
    
    buffer = malloc(size);

    size_t num = fread(buffer,1,size,configFile);

    //printf("Read %d bytes, config = %s",num, buffer);
    if(num < 0)
    {
        TEST_FAIL_MESSAGE("Cannot read file");
    } 
}

void test_phev_config_bootstrap(void)
{

    phevConfig_t * config = phev_config_parseConfig(buffer);

    TEST_ASSERT_NOT_NULL(config);
}

void test_phev_config_updateConfig(void)
{

    phevConfig_t * config = phev_config_parseConfig(buffer);

    TEST_ASSERT_NOT_NULL(config);
    TEST_ASSERT_NOT_NULL(config->updateConfig.updatePath);
    TEST_ASSERT_NOT_NULL(config->updateConfig.updateHost);
    TEST_ASSERT_NOT_NULL(config->updateConfig.updateImageFullPath);
    
    TEST_ASSERT_EQUAL_UINT32(999,config->updateConfig.latestBuild);
    TEST_ASSERT_EQUAL_STRING("storage.googleapis.com",config->updateConfig.updateHost);
    TEST_ASSERT_EQUAL_INT(80, config->updateConfig.updatePort);
    TEST_ASSERT_EQUAL_STRING("/espimages/develop/",config->updateConfig.updatePath);
    TEST_ASSERT_EQUAL_STRING("/espimages/develop/firmware-0000000999.bin",config->updateConfig.updateImageFullPath);
    TEST_ASSERT_TRUE(config->updateConfig.updateOverPPP);
    TEST_ASSERT_TRUE(config->updateConfig.forceUpdate);
    
}

void test_phev_config_updateConfig_wifi(void)
{

    phevConfig_t * config = phev_config_parseConfig(buffer);

    TEST_ASSERT_NOT_NULL(config);
    TEST_ASSERT_NOT_NULL(config->updateConfig.updateWifi.ssid);
    TEST_ASSERT_NOT_NULL(config->updateConfig.updateWifi.password);

    TEST_ASSERT_EQUAL_STRING("ssid",config->updateConfig.updateWifi.ssid);
    TEST_ASSERT_EQUAL_STRING("password",config->updateConfig.updateWifi.password);
    
}

void test_phev_config_connection(void)
{

    phevConfig_t * config = phev_config_parseConfig(buffer);

    TEST_ASSERT_NOT_NULL(config);
    TEST_ASSERT_NOT_NULL(config->connectionConfig.host);
    TEST_ASSERT_NOT_NULL(config->connectionConfig.carConnectionWifi.ssid);
    TEST_ASSERT_NOT_NULL(config->connectionConfig.carConnectionWifi.password);
    
    TEST_ASSERT_EQUAL_STRING("192.168.8.46",config->connectionConfig.host);
    
    TEST_ASSERT_EQUAL_INT(8080,config->connectionConfig.port);

    TEST_ASSERT_EQUAL_STRING("REMOTE123456",config->connectionConfig.carConnectionWifi.ssid);
    TEST_ASSERT_EQUAL_STRING("abcde123456",config->connectionConfig.carConnectionWifi.password);
    
}