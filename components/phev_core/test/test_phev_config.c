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
    TEST_ASSERT_EQUAL_UINT32(999,config->updateConfig.latestBuild);
}