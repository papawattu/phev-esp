#ifndef SETUP_UI_H_
#define SETUP_UI_H_

#include <esp_http_server.h>
#include "phev_core.h"
#include "phev_config.h"

#define SETUP_CONNECTION_CONFIG_JSON "carConnection"
#define SETUP_CONNECTION_CONFIG_HOST "host"
#define SETUP_CONNECTION_CONFIG_PORT "port"
#define SETUP_CONNECTION_CONFIG_SSID "ssid"
#define SETUP_CONNECTION_CONFIG_PASSWORD "password"

#define SETUP_PPP_CONFIG_JSON "pppConnection"
#define SETUP_PPP_CONFIG_USER "user"
#define SETUP_PPP_CONFIG_PASSWORD "password"
#define SETUP_PPP_CONFIG_APN "apn"

#define DEFAULT_CAR_HOST_IP "192.168.8.46"
#define DEFAULT_CAR_HOST_PORT 8080
#define DEFAULT_PPP_USER "eesecure"
#define DEFAULT_PPP_PASSWORD "secure"
#define DEFAULT_PPP_APN "everywhere"


typedef struct connectionDetails_t {
    phevWifi_t wifi;
    char * host;
    int port;
    char * pppUser;
    char * pppPassword;
    char * pppAPN;
} connectionDetails_t;

httpd_handle_t * start_webserver(void);
void stop_webserver(httpd_handle_t server);

connectionDetails_t * setup_ui_getConnectionDetails(void);

#endif 