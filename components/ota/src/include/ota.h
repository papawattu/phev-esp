#ifndef _OTA_H_
#define _OTA_H_
#include <stdint.h>
#define BUFFSIZE 1024
#define TEXT_BUFFSIZE 1024

const static char *APP_TAG = "OTA";

void ota(const char * updateHost,const uint16_t updatePort, const char * updateImageFullPath);
#endif