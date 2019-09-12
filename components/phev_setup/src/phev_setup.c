#include "phev_setup.h"
#include "phev_config.h"
#include "phev_store.h"
#include "ppp_client.h"
#include "phev_register.h"
#include "wifi_client.h"
#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_system.h>
#include "freertos/event_groups.h"
#include <nvs_flash.h>
#include <sys/param.h>
#include "esp_http_client.h"
#include "gcp_jwt.h"

static const char *TAG="PHEV_SETUP";

const static int CONNECTED_BIT = BIT0;
const static int CONFIGURED_BIT = BIT1;

static EventGroupHandle_t setup_event_group;

#define SETUP_CONNECTION_CONFIG_JSON "carConnection"

extern const char phev_remote_config_start[] asm("_binary_phev_remote_config_html_start");
extern const char phev_remote_config_end[]   asm("_binary_phev_remote_config_html_end");

extern const char phev_default_config_start[] asm("_binary_config_json_start");
extern const char phev_default_config_end[] asm("_binary_config_json_end");


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
        config->port = (uint16_t) phev_config_getConfigInt(connection, SETUP_CONNECTION_CONFIG_PORT);
    } 
    else 
    {
        config->port = (uint16_t) DEFAULT_CAR_HOST_PORT;
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
    if(phev_config_checkForOption(json, SETUP_EMAIL)) 
    {
        details->email = strdup(phev_config_getConfigString(json, SETUP_EMAIL));
    } 
    else 
    {
        ESP_LOGE(TAG,"No email in config");
        return NULL;
    } 
    cJSON * carConnection = cJSON_GetObjectItemCaseSensitive(json, SETUP_CONNECTION_CONFIG_JSON);

    phev_setup_parseConnectionConfig(details, carConnection);

    cJSON * pppConnection = cJSON_GetObjectItemCaseSensitive(json, SETUP_PPP_CONFIG_JSON);

    phev_setup_parsePPPConfig(details, pppConnection);

    return details;
}
esp_err_t phev_setup_httpEventHandler(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // Write out data
                printf("%.*s", evt->data_len, (char*)evt->data);
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}
void phev_setup_parseDeviceResponse(phevStore_t * store, const char * response)
{
    cJSON * json = cJSON_Parse((const char *) response);

    if(json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            printf("Error before: %s\n", error_ptr);
        }
        return NULL;
    }
    if(phev_config_checkForOption(json, GCP_PROJECTID)) 
    {
        store->config->gcpProjectId = strdup(phev_config_getConfigString(json, GCP_PROJECTID));
    } 
    else 
    {
        ESP_LOGE(TAG,"Missing project id");
    }
    if(phev_config_checkForOption(json, GCP_LOCATION)) 
    {
        store->config->gcpLocation = strdup(phev_config_getConfigString(json, GCP_LOCATION));
    } 
    else 
    {
        ESP_LOGE(TAG,"Missing location");
    }
    if(phev_config_checkForOption(json, GCP_REGISTRY)) 
    {
        store->config->gcpRegistry = strdup(phev_config_getConfigString(json, GCP_REGISTRY));
    } 
    else 
    {
        ESP_LOGE(TAG,"Missing registry");
    }
    if(phev_config_checkForOption(json, GCP_EVENTS_TOPIC)) 
    {
        store->config->eventsTopic = strdup(phev_config_getConfigString(json, GCP_EVENTS_TOPIC));
    } 
    else 
    {
        ESP_LOGE(TAG,"Missing events topic");
    }
    if(phev_config_checkForOption(json, GCP_STATE_TOPIC)) 
    {
        store->config->stateTopic = strdup(phev_config_getConfigString(json, GCP_STATE_TOPIC));
    } 
    else 
    {
        ESP_LOGE(TAG,"Missing state topic");
    }
    if(phev_config_checkForOption(json, GCP_COMMANDS_TOPIC)) 
    {
        store->config->commandsTopic = strdup(phev_config_getConfigString(json, GCP_COMMANDS_TOPIC));
    } 
    else 
    {
        ESP_LOGE(TAG,"Missing commands topic");
    }
    if(phev_config_checkForOption(json, GCP_CONFIG_TOPIC)) 
    {
        store->config->configTopic = strdup(phev_config_getConfigString(json, GCP_CONFIG_TOPIC));
    } 
    else 
    {
        ESP_LOGE(TAG,"Missing config topic");
    }
}

void phev_setup_getDevice(phevStore_t * store)
{
    char * auth = NULL;
    ESP_LOGI(TAG,"Header Buffer %d",HEADER_BUFFER);
    ESP_LOGI(TAG,"Default Buffer size %d",DEFAULT_HTTP_BUF_SIZE);
    ESP_LOGI(TAG,"Max http recv %d",MAX_HTTP_RECV_BUFFER);

    esp_http_client_config_t config = {
        .buffer_size = 2048,
    };
    
    asprintf(&config.url,"https://us-central1-phev-db3fa.cloudfunctions.net/devices/%s",store->deviceId);

    esp_http_client_handle_t client = esp_http_client_init(&config);
    asprintf(&auth,"Bearer %s\n",createJwt(NULL,store->email));
    ESP_LOGD(TAG,"Auth header : %s", auth);
    esp_http_client_set_header(client, "Authorization", auth);
    
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTPS Status = %d, content_length = %d",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
        int content_length =  esp_http_client_get_content_length(client);
        
        char *buffer = malloc(MAX_HTTP_RECV_BUFFER);
        
        int read_len = esp_http_client_read(client, buffer, content_length);
        buffer[read_len] = 0;
        ESP_LOGD(TAG,"Buffer %s",buffer);
        if(esp_http_client_get_status_code(client) == 200)
        {   
            phev_setup_parseDeviceResponse(store,buffer);
        } else {
            ESP_LOGW(TAG,"Unhandled server response %d",esp_http_client_get_status_code(client));
        }
    } else {
        ESP_LOGE(TAG, "Error perform http request %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}
void phev_setup_register(phevStore_t * store) 
{
    ESP_LOGI(TAG,"Registering...");
    if(store->config) 
    {
        //asprintf(&store->email,"jamie@wattu.com");
        phev_setup_getDevice(store);
        if(store->registered)
        {
            ESP_LOGI(TAG,"Device %s Registered",store->deviceId);
        } else {
            phev_register_start(store);

        }
    } else {
        ESP_LOGW(TAG,"No config found");
    }
    
}
void phev_setup_waitForConfig(phevStore_t * store)
{
    ESP_LOGI(TAG, "Checking for config");
    if(!store->configured) {
        ESP_LOGI(TAG, "Waiting for config...");

        xEventGroupWaitBits(setup_event_group, CONFIGURED_BIT,
                    false, true, portMAX_DELAY);
    
    }
    if(!store->config) {

        ESP_LOGW(TAG, "Config flag set but no config");

        xEventGroupWaitBits(setup_event_group, CONFIGURED_BIT,
                        false, true, portMAX_DELAY);
    } 
    
    ESP_LOGI(TAG, "Config available");
    
}
void phev_setup_startWifiConnection(phevStore_t * store)
{
    
    phev_setup_waitForConfig(store);
    
    ESP_LOGI(TAG, "Connecting to car wifi...");

    wifi_conn_init(store->config->wifi.ssid, store->config->wifi.password, false);

    ESP_LOGI(TAG, "Connected to car wifi");

}
void phev_setup_startPPPConnection(phevStore_t * store)
{
    
    phev_setup_waitForConfig(store);
    
    ESP_LOGI(TAG, "PPP starting...");
    
    pppConnectionDetails_t connectionDetails = {
        .user = store->config->pppUser,
        .password = store->config->pppPassword,
        .apn = store->config->pppAPN,
    };
    
    ppp_main(&connectionDetails);

    ESP_LOGI(TAG, "PPP started");


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
    phevStore_t * store = req->user_ctx;
    phevStoreConnectionConfig_t * config = malloc(sizeof(phevStoreConnectionConfig_t));

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

    if(details == NULL) 
    {
        details = phev_setup_jsonToConnectionDetails(phev_default_config_start);
    }

    ESP_LOGI(TAG, "Email %s",details->email);
    ESP_LOGI(TAG, "Car details");
    
    ESP_LOGI(TAG, "Host %s",details->host);
    ESP_LOGI(TAG, "Port %u",details->port);
    ESP_LOGI(TAG, "SSID %s",details->wifi.ssid);
    ESP_LOGI(TAG, "Password %s",details->wifi.password);

    ESP_LOGI(TAG, "PPP / GSM details");
    
    ESP_LOGI(TAG, "PPP User %s",details->pppUser);
    ESP_LOGI(TAG, "PPP Password %s",details->pppPassword);
    ESP_LOGI(TAG, "PPP APN %s",details->pppAPN);
    
    store->email = strdup(details->email);
    config->host = details->host;
    config->port = details->port;
    strcpy(config->wifi.ssid,details->wifi.ssid);
    strcpy(config->wifi.password,details->wifi.password);
    config->pppUser = details->pppUser;
    config->pppPassword = details->pppPassword;
    config->pppAPN = details->pppAPN;
    
    phev_store_storeConnectionConfig(store,config);
    ESP_LOGI(TAG, "Configured...");
    
    xEventGroupSetBits(setup_event_group, CONFIGURED_BIT);

    return ESP_OK;
}

httpd_uri_t send = {
    .uri       = "/send",
    .method    = HTTP_POST,
    .handler   = post_handler,
    .user_ctx  = NULL
};


httpd_handle_t * phev_setup_startWebserver(phevStore_t * store)
{
    setup_event_group = xEventGroupCreate();
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    send.user_ctx = (void *) store;
    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &home);
        httpd_register_uri_handler(server, &send);

        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

void phev_setup_stopWebserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}