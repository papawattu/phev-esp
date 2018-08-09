#include "tcp_client.h"
#include "logger.h"

static int global_sock = 0;
const static char *APP_TAG = "TCP_CLIENT";

void my_ms_to_timeval(int timeout_ms, struct timeval *tv)
{
    tv->tv_sec = timeout_ms / 1000;
    tv->tv_usec = (timeout_ms - (tv->tv_sec * 1000)) * 1000;
}
static int tcp_poll_read(int soc, int timeout_ms)
{
    fd_set readset;
    FD_ZERO(&readset);
    FD_SET(soc, &readset);
    struct timeval timeout;
    my_ms_to_timeval(timeout_ms, &timeout);
    return select(soc + 1, &readset, NULL, NULL, &timeout);
}
static int tcp_read(int soc, uint8_t *buffer, int len, int timeout_ms)
{
    int poll = -1;
    if ((poll = tcp_poll_read(soc, timeout_ms)) <= 0) {
        return poll;
    }
    int read_len = TCP_READ(soc, buffer, len);
    if (read_len == 0) {
        return -1;
    }
    return read_len;
}

int tcp_client_connectSocket(const char *host, uint16_t port) 
{
    LOG_V(APP_TAG,"START - connectSocket");
    LOG_D(APP_TAG,"Host %s, Port %d",host,port);

    if(host == NULL) 
    {
        LOG_E(APP_TAG,"Host not set");
        return -1;
    }    
    struct sockaddr_in addr;
    /* set up address to connect to */
    memset(&addr, 0, sizeof(addr));
    //addr.sin_len = sizeof(addr);
    addr.sin_family = AF_INET;
    addr.sin_port = TCP_HTONS(port);
    addr.sin_addr.s_addr = inet_addr(host);

    LOG_I(APP_TAG,"Host %s Port %d",host,port);
  
    int sock = TCP_SOCKET(AF_INET, SOCK_STREAM, 0);

    if (sock == -1)
    {
        LOG_E(APP_TAG,"Failed to open socket");
  
        return -1;
    }
    int ret = TCP_CONNECT(sock, (struct sockaddr *)(&addr), sizeof(addr));
    if(ret == -1)
    {
        LOG_E(APP_TAG,"Failed to connect");
  
        return -1;
    }
  
    LOG_I(APP_TAG,"Connected to host %s port %d",host,port);
    
    global_sock = sock;
    LOG_V(APP_TAG,"END - connectSocket");
    
    return sock;
}

int tcp_client_read(int soc, uint8_t * buf, size_t len)
{
    LOG_V(APP_TAG,"START - read");
    
    int num = tcp_read(soc,buf,len,TCP_READ_TIMEOUT);

    LOG_D(APP_TAG,"Read %d bytes from tcp stream", num);
    
    LOG_V(APP_TAG,"END - read");
    
    return num;
}
int tcp_client_write(int soc, uint8_t * buf, size_t len)
{
    LOG_V(APP_TAG,"START - write");
    
    int num = TCP_WRITE(soc,buf,len);
    
    LOG_D(APP_TAG,"Wriiten %d bytes from tcp stream", num);
    
    LOG_V(APP_TAG,"END - write");
    
    return num;
}
