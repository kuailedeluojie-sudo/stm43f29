#ifndef __UPDATE_H
#define __UPDATE_H
#include <rtthread.h>
#include "board.h"

enum Status_code {
    NONE = 0x00,
    SOH  = 0x01,
    STX  = 0x02,
    EOT  = 0x04,
    ACK  = 0x06,
    NAK  = 0x15,
    CAN  = 0x18,
    CODE_C    = 0x43,
};
#define MAGIC 0x11223344
#define HEADER_LEN 3
#define RBIGU4(p)       ((p)[0]<<24|(p)[1]<<16|(p)[2]<<8|(p)[3])
#define WBIGU4(p,i)     do{(p)[0]=((i>>24)&0xffu);(p)[1]=((i>>16)&0xffu);(p)[2]=((i>>8)&0xffu);(p)[3]=(i&0xffu);}while(0)
#define GET_DATA_LENGTH(buf)     (buf+HEADER_LEN)
#define BUF_SZ		4096
#define TCP_UPDATE_PORT      (8888)
#define MAX_DATA_SIZE       (1024)
#define UPDATE_INFO_ADDR 897
#define UPDATE_DATA_ADDR 898
void tcp_update_init(void);
void delete_tcp_update_init(void);
int http_update(const char* url);

#endif 

