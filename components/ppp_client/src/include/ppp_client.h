#ifndef _PPP_CLIENT_H_
#define _PPP_CLIENT_H_

#include "freertos/task.h"

typedef struct pppConnectionDetails_t {
    char * user;
    char * password;
    char * apn;
} pppConnectionDetails_t;

void ppp_main(pppConnectionDetails_t *);

#endif