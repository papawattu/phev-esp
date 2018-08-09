#ifndef _WIFI_CLIENT_H_
#define _WIFI_CLIENT_H_
#include <stdbool.h>

#define MAX_WIFI_CLIENT_SSID_LEN 32
#define MAX_WIFI_CLIENT_PASSWORD_LEN 64

void wifi_conn_init(const char * wifiSSID, const char * wifiPassword, const bool setPPPdefault);
void wifi_client_setup(void);
#endif