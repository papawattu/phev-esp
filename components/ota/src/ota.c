#include <string.h>
#include <stdlib.h>
#include "ota.h"
#include "lwip/netif.h"
#include "lwip/opt.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "esp_log.h"


static int socket_id = -1;

static char ota_write_data[BUFFSIZE + 1] = { 0 };
/*an packet receive buffer*/
static char text[BUFFSIZE + 1] = { 0 };

static int binary_file_length = 0;

static int read_until(char *buffer, char delim, int len)
{
//  /*TODO: delim check,buffer check,further: do an buffer length limited*/
    int i = 0;
    while (buffer[i] != delim && i < len) {
        ++i;
    }
    return i + 1;
}

/* resolve a packet from http socket
 * return true if packet including \r\n\r\n that means http packet header finished,start to receive packet body
 * otherwise return false
 * */
static bool read_past_http_header(char text[], int total_len, esp_ota_handle_t update_handle)
{
    /* i means current position */
    int i = 0, i_read_len = 0;
    while (text[i] != 0 && i < total_len) {
        i_read_len = read_until(&text[i], '\n', total_len);
        // if we resolve \r\n line,we think packet header is finished
        if (i_read_len == 2) {
            int i_write_len = total_len - (i + 2);
            memset(ota_write_data, 0, BUFFSIZE);
            /*copy first http packet body to write buffer*/
            memcpy(ota_write_data, &(text[i + 2]), i_write_len);

            esp_err_t err = esp_ota_write( update_handle, (const void *)ota_write_data, i_write_len);
            if (err != ESP_OK) {
                ESP_LOGE(APP_TAG, "Error: esp_ota_write failed (%s)!", esp_err_to_name(err));
                return false;
            } else {
                ESP_LOGI(APP_TAG, "esp_ota_write header OK");
                binary_file_length += i_write_len;
            }
            return true;
        }
        i += i_read_len;
    }
    return false;
}
static bool connect_to_http_server(const char * host, const uint16_t port)
{
    ESP_LOGI(APP_TAG, "Server IP: %s Server Port:%d", host, port);

    int  http_connect_flag = -1;
    struct sockaddr_in sock_info;
    struct addrinfo *res;
    struct in_addr *addr;
    
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };

    int err = getaddrinfo(host , "80", &hints, &res);

    if(err != 0 || res == NULL) {
        ESP_LOGE(APP_TAG, "DNS lookup failed err=%d res=%p", err, res);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        return false;
     }

        /* Code to print the resolved IP.

           Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
    addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
    ESP_LOGI(APP_TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

    socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_id == -1) {
        ESP_LOGE(APP_TAG, "Create socket failed!");
        return false;
    }

    // set connect info
    memset(&sock_info, 0, sizeof(struct sockaddr_in));
    sock_info.sin_family = AF_INET;
    sock_info.sin_addr = *addr;
    sock_info.sin_port = htons(port);

    // connect to http server
    http_connect_flag = connect(socket_id, (struct sockaddr *)&sock_info, sizeof(sock_info));
    if (http_connect_flag == -1) {
        ESP_LOGE(APP_TAG, "Connect to server failed! errno=%d", errno);
        close(socket_id);
        return false;
    } else {
        ESP_LOGI(APP_TAG, "Connected to server");
        return true;
    }
    return false;
}

void ota(const char * updateHost,const uint16_t updatePort, const char * updateImageFullPath)
{

    esp_err_t err;
    /* update handle : set by esp_ota_begin(), must be freed via esp_ota_end() */
    esp_ota_handle_t update_handle = 0 ;
    const esp_partition_t *update_partition = NULL;

    ESP_LOGI(APP_TAG, "Starting OTA ...");

    const esp_partition_t *configured = esp_ota_get_boot_partition();
    const esp_partition_t *running = esp_ota_get_running_partition();

    if (configured != running) {
        ESP_LOGW(APP_TAG, "Configured OTA boot partition at offset 0x%08x, but running from offset 0x%08x",
                 configured->address, running->address);
        ESP_LOGW(APP_TAG, "(This can happen if either the OTA boot data or preferred boot image become corrupted somehow.)");
    }
    ESP_LOGI(APP_TAG, "Running partition type %d subtype %d (offset 0x%08x)",
             running->type, running->subtype, running->address);

    /* Wait for the callback to set the CONNECTED_BIT in the
       event group.
    */
    
    /*connect to http server*/
    if (connect_to_http_server(updateHost,updatePort)) {
        ESP_LOGI(APP_TAG, "Connected to http server");
    } else {
        ESP_LOGE(APP_TAG, "Connect to http server failed!");
        return;
    }

    /*send GET request to http server*/
    const char *GET_FORMAT =
        "GET %s HTTP/1.0\r\n"
        "Host: %s:%d\r\n"
        "User-Agent: esp-idf/1.0 esp32\r\n\r\n";

    char *http_request = NULL;
    int get_len = asprintf(&http_request, GET_FORMAT, updateImageFullPath, updateHost, updatePort);
    if (get_len < 0) {
        ESP_LOGE(APP_TAG, "Failed to allocate memory for GET request buffer");
        return;
    }
    int res = send(socket_id, http_request, get_len, 0);
    free(http_request);

    if (res < 0) {
        ESP_LOGE(APP_TAG, "Send GET request to server failed %s", http_request);
        return;
    } else {
        ESP_LOGI(APP_TAG, "Send GET request to server succeeded");
    }

    update_partition = esp_ota_get_next_update_partition(NULL);
    ESP_LOGI(APP_TAG, "Writing to partition subtype %d at offset 0x%x",
             update_partition->subtype, update_partition->address);
    assert(update_partition != NULL);

    err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &update_handle);
    if (err != ESP_OK) {
        ESP_LOGE(APP_TAG, "esp_ota_begin failed (%s)", esp_err_to_name(err));
        return;
    }
    ESP_LOGI(APP_TAG, "esp_ota_begin succeeded");

    bool resp_body_start = false, socket_flag = true, http_200_flag = false;
    /*deal with all receive packet*/
    while (socket_flag) {
        memset(text, 0, TEXT_BUFFSIZE);
        memset(ota_write_data, 0, BUFFSIZE);
        int buff_len = recv(socket_id, text, TEXT_BUFFSIZE, 0);
        if (buff_len < 0) { /*receive error*/
            ESP_LOGE(APP_TAG, "Error: receive data error! errno=%d", errno);
            return;
        } else if (buff_len > 0 && !resp_body_start) {  /*deal with response header*/
            // only start ota when server response 200 state code
            if (strstr(text, "200") == NULL && !http_200_flag) {
                ESP_LOGE(APP_TAG, "ota url is invalid or bin is not exist");
                return;
            }
            http_200_flag = true;
            memcpy(ota_write_data, text, buff_len);
            resp_body_start = read_past_http_header(text, buff_len, update_handle);
        } else if (buff_len > 0 && resp_body_start) { /*deal with response body*/
            memcpy(ota_write_data, text, buff_len);
            err = esp_ota_write( update_handle, (const void *)ota_write_data, buff_len);
            if (err != ESP_OK) {
                ESP_LOGE(APP_TAG, "Error: esp_ota_write failed (%s)!", esp_err_to_name(err));
                return;
            }
            binary_file_length += buff_len;
            ESP_LOGD(APP_TAG, "Have written image length %d", binary_file_length);
        } else if (buff_len == 0) {  /*packet over*/
            socket_flag = false;
            ESP_LOGI(APP_TAG, "Connection closed, all packets received");
            close(socket_id);
        } else {
            ESP_LOGE(APP_TAG, "Unexpected recv result");
        }
    }

    ESP_LOGI(APP_TAG, "Total Write binary data length : %d", binary_file_length);

    if (esp_ota_end(update_handle) != ESP_OK) {
        ESP_LOGE(APP_TAG, "esp_ota_end failed!");
        return;
    }
    err = esp_ota_set_boot_partition(update_partition);
    if (err != ESP_OK) {
        ESP_LOGE(APP_TAG, "esp_ota_set_boot_partition failed (%s)!", esp_err_to_name(err));
        return;
    }
    ESP_LOGI(APP_TAG, "Prepare to restart system!");
    esp_restart();
    return ;
}
