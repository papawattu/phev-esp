#include "phev_register.h"
#include "wifi_client.h"
#include "tcp_client.h"

#include "logger.h"
#include <string.h>

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

const static char * TAG = "PHEV_REGISTER";

void phev_register_start(phevStore_t * store)
{ 

    char rx_buffer[128];
    LOG_V(TAG,"START - phev register start");
    wifi_conn_initAndWait(store->config->wifi.ssid,store->config->wifi.password,true);
    //wifi_conn_initAndWait("BTHub6-P535","S1mpsons",false);
    int socket = tcp_client_connectSocket(store->config->host, store->config->port); 
    while(1) {
        int len = recv(socket, rx_buffer, sizeof(rx_buffer) - 1, 0);
            // Error occured during receiving
        if (len < 0) {
            LOG_E(TAG, "recv failed: errno %d", errno);
        } else {
            rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
            LOG_I(TAG, "Received %d bytes", len);
            LOG_I(TAG, "%s", rx_buffer);
            LOG_BUFFER_HEXDUMP(TAG,rx_buffer,len,LOG_DEBUG);
        }
    }    
    
    LOG_V(TAG,"END - phev register end");

}