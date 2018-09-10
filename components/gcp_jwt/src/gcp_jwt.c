#include <stdint.h>
#include <time.h>
#ifndef __linux__ 
#include "gcp_jwt.h"
#include "jwt.h"
#include "logger.h"

const static char *APP_TAG = "GCP_JWT";

extern const uint8_t rsa_private_pem_start[] asm("_binary_rsa_private2_pem_start");
extern const uint8_t rsa_private_pem_end[]   asm("_binary_rsa_private2_pem_end");

void getIatExp(char *iat, char *exp, int time_size)
{
    LOG_V(APP_TAG,"START - getIatExp");
    
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    LOG_D(APP_TAG,"Waiting for time to be set");
    //while (timeinfo.tm_year < (2016 - 1900))
    //{
        time(&now);
        localtime_r(&now, &timeinfo);
    //}
    LOG_D(APP_TAG,"Time set");
    
    snprintf(iat, time_size, "%lu", now);
    snprintf(exp, time_size, "%lu", now + 3600);
    LOG_V(APP_TAG,"END - getIatExp");
    
} 
char *createJwt(const char *project_id)
{
    LOG_V(APP_TAG,"START - createJwt");
    
    char iat_time[sizeof(time_t) * 3 + 2];
    char exp_time[sizeof(time_t) * 3 + 2];
    const uint8_t *key = (unsigned char *) rsa_private_pem_start;
    size_t key_len = rsa_private_pem_end - rsa_private_pem_start;
    jwt_t *jwt = NULL;
    int ret = 0; 
    char *out = NULL;

    getIatExp(iat_time, exp_time, sizeof(iat_time));
    
    jwt_new(&jwt);

    ret = jwt_add_grant(jwt, "iat", iat_time);
    if (ret)
    {
        LOG_E(APP_TAG,"Error setting issue timestamp: %d", ret);
        return NULL;
    }
    ret = jwt_add_grant(jwt, "exp", exp_time);
    if (ret)
    {
        LOG_E(APP_TAG,"Error setting expiration: %d", ret);
        return NULL;
    }
    ret = jwt_add_grant(jwt, "aud", project_id);
    if (ret)
    {
        LOG_E(APP_TAG,"Error adding audience: %d", ret);
        return NULL;
    }
    ret = jwt_set_alg(jwt, JWT_ALG_RS256, key, key_len);
    if (ret)
    {
        LOG_E(APP_TAG,"Error during set alg: %d",ret);
    
        return NULL;
    }
    out = jwt_encode_str(jwt);
    LOG_D(APP_TAG,"Created JWT %s",out);
    jwt_free(jwt);  
    LOG_V(APP_TAG,"END - createJwt");
    
    return out;
}
#else
char *createJwt(const char *project_id)
{
    return "";
}
#endif