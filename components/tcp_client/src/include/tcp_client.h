#ifndef _TCP_H_
#define _TCP_H_
#include <stdint.h>
#include <stdlib.h>

#ifndef __linux__

//ESP
#include "lwip/opt.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "lwip/netif.h"

#define TCP_READ lwip_read
#define TCP_WRITE lwip_write
#define TCP_CONNECT lwip_connect
#define TCP_SOCKET lwip_socket
#define TCP_HTONS PP_HTONS

#else

// Linux
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>

#define TCP_READ read
#define TCP_WRITE write
#define TCP_CONNECT connect
#define TCP_SOCKET socket
#define TCP_HTONS htons
#endif

#define TCP_READ_TIMEOUT 1000

int tcp_client_connectSocket(const char *host, uint16_t port); 

int tcp_client_read(int soc, uint8_t * buf, size_t len);

int tcp_client_write(int soc, uint8_t * buf, size_t len);

#endif
