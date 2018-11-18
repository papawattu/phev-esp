#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "logger.h"

void hexdump(char * tag, unsigned char * buffer, int length, int level)
{
    if(length <= 0 || buffer == NULL) return;

    char out[17];
    memset(&out,'\0',17);
        
    printf("%s: ",tag);
    int i = 0;
    for(i=0;i<length;i++)
    {
        printf("%02x ",buffer[i]);
        out[i % 16] = (isprint(buffer[i]) ? buffer[i] : '.');
        if((i+1) % 8 == 0) printf(" ");
        if((i+1) % 16 ==0) {
            out[16] = '\0';
            printf(" | %s |\n%s: ",out,tag);
        }
    }
    if((i % 16) + 1 != 0)
    {
        int num = (16 - (i % 16)) * 3;
        num = ((i % 16) < 8 ? num + 1 : num);
        out[(i % 16)] = '\0';
        char padding[(16 * 3) + 2];
        memset(&padding,' ',num+1);
        padding[(16-i)*3] = '\0';
        printf("%s | %s |\n",padding,out);
    }
    printf("\n");
}/*
void msgBundleDump(const char * tag, messageBundle_t * bundle)
{
    for(int i = 0;i<bundle->numMessages;i++)
    {
        LOG_D(tag,"Message %d",i);
        LOG_BUFFER_HEXDUMP(tag,bundle->messages[i]->data,bundle->messages[i]->length,LOG_DEBUG);            
    }
}


*/