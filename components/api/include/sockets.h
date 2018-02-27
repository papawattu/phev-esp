#ifdef TEST

#define AF_INET 0
#define PF_INET 0
#define SOCK_STREAM 0

struct sockaddr {};
struct sockaddr_in {
    int sin_family;
    int sin_port;
    int sin_addr;
};

void bzero(int *x,size_t s);
int htons(int a);
void inet_aton(char *host, int* b);
int socket(int a,int b,int c);
int connect(int a,struct sockaddr * b,size_t c);
ssize_t write(int fd, const void *buf, size_t count);
ssize_t read(int fd, void *buf, size_t count);
#endif