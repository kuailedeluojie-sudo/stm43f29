#include "lwip/sockets.h"
#include "lwip/tcpip.h"
#include "lwip/ip_addr.h"
#include "lwip/inet.h"
#include "update.h"
#include <finsh.h>
static rt_device_t flash_dev=RT_NULL;

static unsigned char header_buf[HEADER_LEN];
static rt_uint32_t total_size=0;
static rt_uint16_t page_offset=0,pack_count=0;

#define SCREEN_WIDTH 80
//static int cur_cursor;
static rt_mutex_t update_lock;


// return: 0  timeout.
//         >0 length
//         <0 socket error
int
blocking_lwip_recv(int s, void *mem, size_t len, int timeout) // the len param here is the length to recv.
{
    int readlen, offset = 0;
    fd_set fds;
    int ret;
    struct timeval to;

    to.tv_sec = timeout / 1000;
    to.tv_usec = (timeout % 1000) * 1000;

    while (1)
    {
        FD_ZERO(&fds);
        FD_SET(s, &fds);
        ret = lwip_select(s + 1, &fds, 0, 0, &to);
        if (ret == 0)
            break;
        else if (ret < 0)
            return ret;

        readlen = lwip_recvfrom(s, ((char *)mem) + offset, len - offset, 0, NULL, NULL);
        if (readlen == 0)  // select is ok and readlen == 0 means connection lost.
            return -1;
        else if (readlen < 0)
            return readlen;
        if (readlen == (len - offset))
        {
            offset = len;
            break;
        }
        offset += readlen;
    }
    return offset;
}

void tcp_update_thread_entry(void *p)
{

    int listenfd,connected;
	            int ret,index,data_index;
    struct sockaddr_in server_addr,client_addr;
    socklen_t sin_size,retry=10;
    rt_uint8_t* data_buf = RT_NULL;
	   	flash_dev = rt_device_find("flash0");
	if (flash_dev==RT_NULL)
	{
		rt_kprintf("can not find the flash device!\n");
    return;
	}
    listenfd = lwip_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenfd == -1)
    {
        rt_kprintf("TCP setup can not create socket!\n");
        return;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(TCP_UPDATE_PORT);

    if (lwip_bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        lwip_close(listenfd);
        rt_kprintf("TCP setup thread socket bind failed!\n");
        return;
    }

    /* Put socket into listening mode */
    if (lwip_listen(listenfd, 2) == -1)
    {
        lwip_close(listenfd);
        rt_kprintf("Listen failed.\n");
        return;
    }
  //  rt_kprintf("tcp update listen to port %d\n",TCP_UPDATE_PORT);
    /* Wait for data or a new connection */
    while (1)
    {
			sin_size = sizeof(struct sockaddr_in);
			connected = lwip_accept(listenfd, (struct sockaddr *)&client_addr, &sin_size);
			/* 接受返回的client_addr指向了客户端的地址信息 */
			// rt_kprintf("I got a connection from (%s , %d)\n",
			//                  inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
			rt_mutex_take(update_lock,RT_WAITING_FOREVER);
			data_buf = rt_malloc (BUF_SZ);
			if (data_buf == RT_NULL)
			{
			rt_kprintf("out of memory\n");
			}          


            // begin to recv & send.
            while (1)
            {
				index = 0;
				while(index<HEADER_LEN)
				{
							  //接收3个字节数据
					ret = lwip_recv(connected, &header_buf[index], HEADER_LEN-index, 500);
							  //接收超时
							 
					if (ret <= 0)
						continue;
					index+=ret;
					if(index<HEADER_LEN)
					{  
						continue;
					}
								//接收错误
					if ((ret != HEADER_LEN)||((header_buf[1]+header_buf[2])!=0xff))
					{
						rt_kprintf("header data error!\n");
						header_buf[0]=NAK;
						if (lwip_send(connected, header_buf, 1, 0) < 0)
						goto exit;
					}	
				}											
                if(header_buf[0]==SOH)
                {
					char buf[4];
					//接收4个字节，总长度
					retry=10;
					data_index=0;
					while(data_index<4&&retry--)
					{
						ret = lwip_recv(connected, &buf[data_index], 4-data_index, 500);
						if(ret==0)
						{
							continue;
						}
						data_index+=ret;
						if(retry==0)
						{
							rt_kprintf("header data error!\n");
							header_buf[0]=NAK;
							if (lwip_send(connected, header_buf, 1, 0) < 0)
							goto exit;
						}
					}
					pack_count=0;
					page_offset=0;

					total_size = RBIGU4(buf);
					rt_kprintf("0x%02x,0x%02x,0x%02x,0x%02x\n",buf[0],buf[1],buf[2],buf[3]);
					rt_kprintf("total size:%d bytes\n",total_size);
					header_buf[0]=ACK;
					if (lwip_send(connected, header_buf, 1, 0) < 0)
					break;
				}
				else if(header_buf[0]==STX)
				{
					retry=20;
					data_index=0;
					//接收一帧数据
					while(data_index<MAX_DATA_SIZE&&retry--)
					{
						ret = blocking_lwip_recv(connected, data_buf+pack_count*MAX_DATA_SIZE+data_index, MAX_DATA_SIZE-data_index, 500);
						if(ret==0)
						{
							continue;
						}
						data_index+=ret;
						if(retry==0)
						{
							header_buf[0]=NAK;
							rt_kprintf("data pack error!\n");
							if (lwip_send(connected, header_buf, 1, 0) < 0)
							goto exit;
						}
					}
					pack_count++;
					if(pack_count==4)
					{//写入到flash中
						pack_count=0;
						flash_dev->write(flash_dev,UPDATE_DATA_ADDR+page_offset,data_buf,1);
						page_offset++;
					}
					//发送ACK等待下一个数据包
					header_buf[0]=ACK;
					if (lwip_send(connected, header_buf, 1, 0) < 0)
					break;
			  }
			  
				else if(header_buf[0]==EOT)
				{ 
					rt_kprintf("data write finish!\n");
					if(pack_count!=0)
					{//写入到flash中
						pack_count=0;
						flash_dev->write(flash_dev,UPDATE_DATA_ADDR+page_offset,data_buf,1);
						page_offset++;
					}
					WBIGU4(data_buf,total_size);
					WBIGU4(data_buf+4,MAGIC);
					flash_dev->write(flash_dev,UPDATE_INFO_ADDR,data_buf,1);
					header_buf[0]=ACK;
					lwip_send(connected, header_buf, 1, 0);
					rt_thread_delay(10);
					/* CPU reset */
					 rt_hw_cpu_reset();
					break;
				}
            }
exit:
			rt_mutex_release(update_lock);
            rt_kprintf("TCP setup disconnected.\n");
            lwip_close(connected);
			if(data_buf!=RT_NULL)
            rt_free(data_buf);
    }// while(1) listen.
}
rt_thread_t tcp_update_thread;
void tcp_update_init(void)
{
  
    update_lock=rt_mutex_create("update",RT_IPC_FLAG_FIFO);
    // TCP update thread.
    tcp_update_thread = rt_thread_create("tcpupdate", tcp_update_thread_entry, RT_NULL, 1024, 16, 5);
	  if(tcp_update_thread!=RT_NULL)
    rt_thread_startup(tcp_update_thread);
}
void delete_tcp_update_init(void)
{
	rt_thread_delete(tcp_update_thread);
	tcp_update_thread=NULL;
	rt_thread_delay(10);
}











