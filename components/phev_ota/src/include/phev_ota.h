#ifndef _PHEV_OTA_H_
#define _PHEV_OTA_H_
#include <stdint.h>
#define BUFFSIZE 1024
#define TEXT_BUFFSIZE 1024

void ota(const char * updateHost,const uint16_t updatePort, const char * updateImageFullPath);
#endif