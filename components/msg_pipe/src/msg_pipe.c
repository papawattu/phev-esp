#include "msg_pipe.h"

void msg_pipe(messagingClient_t *in, messagingClient_t *out) 
{
    in->start(in);
    in->connect(in);

    out->start(out);
    out->connect(out);

    
}