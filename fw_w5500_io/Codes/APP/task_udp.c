#include "task_udp.h"

#include <string.h>

#include "socket.h"
#include "task_udp.h"
#include "freertos.h"
#include "task.h"
#include "semphr.h"
#include "mew_board.h"
#include "cjson.h"

SemaphoreHandle_t sem_udp;

uint8_t dest_ip[4] = {192, 168, 1, 100};
uint16_t dest_port = 9000;

void parse_cmd(char const *buff)
{
	cJSON *json;
	cJSON *type, *rly1, *rly2, *rly3, *rly4;
	
	json = cJSON_Parse(buff);
	type = cJSON_GetObjectItem(json, "type");
	rly1 = cJSON_GetObjectItem(json, "rly1");
	rly2 = cJSON_GetObjectItem(json, "rly2");
	rly3 = cJSON_GetObjectItem(json, "rly3");
	rly4 = cJSON_GetObjectItem(json, "rly4");
	
	uint8_t msg = 0;
	
	if(type != NULL && strcmp(type->valuestring, "cmd") == 0)
	{
		if(rly1 != NULL)
		{
			msg = 0;
			if(strcmp(rly1->valuestring, "close") == 0)
			{
				msg = 0x80;
				xQueueSend(que_rly, &msg, portMAX_DELAY);
			}
			if(strcmp(rly1->valuestring, "open") == 0)
			{
				xQueueSend(que_rly, &msg, portMAX_DELAY);
			}
			cJSON_Delete(rly1);
		}
		if(rly2 != NULL)
		{
			msg = 1;			
			if(strcmp(rly2->valuestring, "close") == 0)
			{
				msg |= 0x80;
				xQueueSend(que_rly, &msg, portMAX_DELAY);
			}
			if(strcmp(rly2->valuestring, "open") == 0)
			{
				xQueueSend(que_rly, &msg, portMAX_DELAY);
			}
			cJSON_Delete(rly2);
		}
		if(rly3 != NULL)
		{
			msg = 2;
			if(strcmp(rly3->valuestring, "close") == 0)
			{
				msg |= 0x80;
				xQueueSend(que_rly, &msg, portMAX_DELAY);
			}
			if(strcmp(rly3->valuestring, "open") == 0)
			{
				xQueueSend(que_rly, &msg, portMAX_DELAY);
			}
			cJSON_Delete(rly3);
		}
		if(rly4 != NULL)
		{
			msg = 3;
			if(strcmp(rly4->valuestring, "close") == 0)
			{
				msg |= 0x80;
				xQueueSend(que_rly, &msg, portMAX_DELAY);
			}
			if(strcmp(rly4->valuestring, "open") == 0)
			{
				xQueueSend(que_rly, &msg, portMAX_DELAY);
			}			
			cJSON_Delete(rly4);
		}
		cJSON_Delete(type);
	}		
	cJSON_Delete(json);
}

void task_udp(void *parm)
{	
	int32_t ret;
	uint8_t *rbuf;
	uint16_t rlen;
	uint8_t src_ip[4];
	uint16_t port;
	
	sem_udp = xSemaphoreCreateBinary();
	
	//打开SOCKET1的接收中断
	sockint_kind sik = SIK_ALL;
	//if(SOCK_OK != ctlsocket(1, CS_SET_INTMASK, &sik))
	{
		//while(1);
	}
	if(SOCK_OF_UDP != socket(SOCK_OF_UDP, Sn_MR_UDP, 502, 0))
	{
		while(1);
	}	
	
	
	while(1)
	{
		if(xSemaphoreTake(sem_udp, portMAX_DELAY) == pdTRUE)
		{			
			if(SOCK_OK != getsockopt(SOCK_OF_UDP, SO_RECVBUF, &rlen))
			{					
			}
			if(rlen >= 8)
			{
				rlen -= 8;
				if(rlen > 0)
				{
					rbuf = pvPortMalloc(rlen);
					if(rbuf != NULL)
					{
						ret = recvfrom(SOCK_OF_UDP, rbuf, rlen, src_ip, &port);
						if(ret == rlen)
						{
							//sendto(SOCK_OF_UDP, rbuf, rlen, src_ip, port);
							parse_cmd((const char *)rbuf);
						}
						vPortFree(rbuf);
					}
					else
					{
						// not have enough memory
					}
				}
				else
				{
					// not have payload data
				}
			}
			// must clr interrupt after read the buffer
//			if(SOCK_OK != ctlsocket(SOCK_OF_UDP, CS_CLR_INTERRUPT, &sik))
//			{
//			}			
			//wizchip_clrinterrupt(IK_SOCK_ALL);
		}		
		//
	}
}
