#ifndef SETUP_UI_H_
#define SETUP_UI_H_

#include <esp_http_server.h>

typedef struct connectionDetails_t {
    char * pppUser;
    char * pppPassword;
    char * pppAPN;
} connectionDetails_t;

httpd_handle_t * start_webserver(void);
void stop_webserver(httpd_handle_t server);

connectionDetails_t * setup_ui_getConnectionDetails(void);

#endif 