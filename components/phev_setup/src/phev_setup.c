#include "phev_setup.h"
#include "phev_config.h"
#include "ppp_client.h"
#include "wifi_client.h"
#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_system.h>
#include "freertos/event_groups.h"
#include <nvs_flash.h>
#include <sys/param.h>

static const char *TAG="PHEV_SETUP";

const static int CONNECTED_BIT = BIT0;

static EventGroupHandle_t setup_event_group;

#define SETUP_CONNECTION_CONFIG_JSON "carConnection"

extern const char phev_remote_config_start[] asm("_binary_phev_remote_config_html_start");
extern const char phev_remote_config_end[]   asm("_binary_phev_remote_config_html_end");

void phev_setup_parseConnectionConfig(connectionDetails_t * config, cJSON * connection)
{
    if(phev_config_checkForOption(connection, SETUP_CONNECTION_CONFIG_HOST)) 
    {
        config->host = phev_core_strdup(phev_config_getConfigString(connection, SETUP_CONNECTION_CONFIG_HOST));
    } 
    else 
    {
        config->host = DEFAULT_CAR_HOST_IP;
    }
    if(phev_config_checkForOption(connection, SETUP_CONNECTION_CONFIG_PORT))
    {
        config->port = phev_config_getConfigInt(connection, SETUP_CONNECTION_CONFIG_PORT);
    } 
    else 
    {
        config->port = DEFAULT_CAR_HOST_PORT;
    }
    
    strcpy(config->wifi.ssid, phev_config_getConfigString(connection, SETUP_CONNECTION_CONFIG_SSID)); 
    strcpy(config->wifi.password, phev_config_getConfigString(connection, SETUP_CONNECTION_CONFIG_PASSWORD)); 
}

void phev_setup_parsePPPConfig(connectionDetails_t * config, cJSON * ppp)
{
    if(phev_config_checkForOption(ppp, SETUP_PPP_CONFIG_USER)) 
    {
        config->pppUser = phev_core_strdup(phev_config_getConfigString(ppp, SETUP_PPP_CONFIG_USER));
    } 
    else 
    {
        config->pppUser = DEFAULT_PPP_USER;
    }
    if(phev_config_checkForOption(ppp, SETUP_PPP_CONFIG_PASSWORD)) 
    {
        config->pppPassword = phev_core_strdup(phev_config_getConfigString(ppp, SETUP_PPP_CONFIG_PASSWORD));
    } 
    else 
    {
        config->pppPassword = DEFAULT_PPP_PASSWORD;
    }
    if(phev_config_checkForOption(ppp, SETUP_PPP_CONFIG_APN)) 
    {
        config->pppAPN = phev_core_strdup(phev_config_getConfigString(ppp, SETUP_PPP_CONFIG_APN));
    } 
    else 
    {
        config->pppAPN = DEFAULT_PPP_APN;
    }

}
connectionDetails_t * phev_setup_jsonToConnectionDetails(const char * config)
{
    connectionDetails_t * details = malloc(sizeof(connectionDetails_t));
    
    cJSON * json = cJSON_Parse((const char *) config);

    if(json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            printf("Error before: %s\n", error_ptr);
        }
        return NULL;
    } 
    cJSON * carConnection = cJSON_GetObjectItemCaseSensitive(json, SETUP_CONNECTION_CONFIG_JSON);

    phev_setup_parseConnectionConfig(details, carConnection);

    cJSON * pppConnection = cJSON_GetObjectItemCaseSensitive(json, SETUP_PPP_CONFIG_JSON);

    phev_setup_parsePPPConfig(details, pppConnection);


    return details;
}
connectionDetails_t * setup_ui_getConnectionDetails() 
{
    connectionDetails_t * details = malloc(sizeof(connectionDetails_t));

    details->pppUser = "eesecure";
    details->pppPassword = "secure";
    details->pppAPN = "everywhere";

    return details;
}


esp_err_t get_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;

    
    /* Send response with custom headers and body set as the
     * string passed in user context*/
    const char* resp_str = (const char*) req->user_ctx;
    httpd_resp_send(req, phev_remote_config_start, phev_remote_config_end - phev_remote_config_start);

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    return ESP_OK;
}

httpd_uri_t home = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = NULL,
};

/* An HTTP POST handler */
esp_err_t post_handler(httpd_req_t *req)
{
    char buf[100];
    int ret, remaining = req->content_len;

    char * request = malloc(req->content_len);
    int pointer = 0;
    while (remaining > 0) {
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buf,
                        MIN(remaining, sizeof(buf)))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry receiving if timeout occurred */
                continue;
            }
            return ESP_FAIL;
        }

        /* Send back the same data */
        //httpd_resp_send_chunk(req, buf, ret);
        remaining -= ret;
        memcpy(request + pointer,buf,ret);
        pointer += ret;
        /* Log data received */
        ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");
        ESP_LOGI(TAG, "%.*s", ret, buf);
        ESP_LOGI(TAG, "====================================");
    }


    // End response
    httpd_resp_send(req, NULL, 0);

    connectionDetails_t * details = phev_setup_jsonToConnectionDetails(request);

    ESP_LOGI(TAG, "Car details");
    
    ESP_LOGI(TAG, "Host %s",details->host);
    ESP_LOGI(TAG, "Port %d",details->port);
    ESP_LOGI(TAG, "SSID %s",details->wifi.ssid);
    ESP_LOGI(TAG, "Password %s",details->wifi.password);

    ESP_LOGI(TAG, "PPP / GSM details");
    
    ESP_LOGI(TAG, "PPP User %s",details->pppUser);
    ESP_LOGI(TAG, "PPP Password %s",details->pppPassword);
    ESP_LOGI(TAG, "PPP APN %s",details->pppAPN);
    
    ESP_LOGI(TAG, "Connecting to car wifi...");

    wifi_conn_init(details->wifi.ssid, details->wifi.password, false);

    ESP_LOGI(TAG, "Connected to car wifi");

#ifndef NO_PPP    
    ESP_LOGD(TAG, "PPP starting...");
    
    pppConnectionDetails_t connectionDetails = {
        .user = details->pppUser,
        .password = details->pppPassword,
        .apn = details->pppAPN,
    };
    
    ppp_main(&connectionDetails);

    ESP_LOGI(TAG, "PPP started");

    xEventGroupSetBits(setup_event_group, CONNECTED_BIT);
/*    
    for (struct netif *pri = netif_list; pri != NULL; pri=pri->next)
    {
        ESP_LOGD(TAG, "Interface priority is %c%c%d (" IPSTR "/" IPSTR " gateway " IPSTR ")",
        pri->name[0], pri->name[1], pri->num,
        IP2STR(&pri->ip_addr.u_addr.ip4), IP2STR(&pri->netmask.u_addr.ip4), IP2STR(&pri->gw.u_addr.ip4));
        if(pri->name[0] == 'p') netif_set_default(pri);
    }
    */ 
#endif

    return ESP_OK;
}

httpd_uri_t send = {
    .uri       = "/send",
    .method    = HTTP_POST,
    .handler   = post_handler,
    .user_ctx  = NULL
};


httpd_handle_t * start_webserver(void)
{
    setup_event_group = xEventGroupCreate();
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &home);
        httpd_register_uri_handler(server, &send);

        xEventGroupWaitBits(setup_event_group, CONNECTED_BIT,
                        false, true, portMAX_DELAY);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}