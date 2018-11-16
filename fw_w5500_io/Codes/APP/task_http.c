#include "task_udp.h"

#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "httpserver.h"
//#include "httpparser.h"
//#include "httputil.h"
#include "task_http.h"
#include "freertos.h"
#include "task.h"
#include "semphr.h"
#include "mew_board.h"
#include "socket.h"
#include "pages.h"
#include "cjson.h"


SemaphoreHandle_t sem_http;

char tmpstr[1024];

uint8_t http_tbuf[2048];
uint8_t http_rbuf[2048];
char *page_w5500_js;

uint8_t http_sn[1] = {SOCK_OF_HTTP};

uint8_t dbg_ip[4] = {192, 168, 1, 100};
uint16_t dbg_port = 9000;

void gen_w5500_js(char **pagep, wiz_NetInfo config_msg)
{
	cJSON *jo;
	jo = cJSON_CreateObject();
	
	cJSON_AddStringToObject(jo, "ver", "1.0");
	sprintf(tmpstr, "%02X:%02X:%02X:%02X:%02X:%02X",\
	config_msg.mac[0], config_msg.mac[1], config_msg.mac[2],\
	config_msg.mac[3], config_msg.mac[4], config_msg.mac[5]);	
	cJSON_AddStringToObject(jo, "mac", tmpstr);
	
	sprintf(tmpstr, "%d.%d.%d.%d",\
	config_msg.ip[0],config_msg.ip[1],config_msg.ip[2],config_msg.ip[3]);
	cJSON_AddStringToObject(jo, "ip", tmpstr);
	
	sprintf(tmpstr, "%d.%d.%d.%d",\
	config_msg.gw[0],config_msg.gw[1],config_msg.gw[2],config_msg.gw[3]);
	cJSON_AddStringToObject(jo, "gw", tmpstr);
	
	sprintf(tmpstr, "%d.%d.%d.%d",\
	config_msg.sn[0],config_msg.sn[1],config_msg.sn[2],config_msg.sn[3]);
	cJSON_AddStringToObject(jo, "sub", tmpstr);
	
	sprintf(tmpstr,	"settingsCallback(%s);",\
	cJSON_PrintUnformatted(jo));
	
	cJSON_Delete(jo);
  
	*pagep = tmpstr;
}

uint8_t predefined_get_cgi_processor(uint8_t * uri_name, uint8_t * buf, uint16_t * len)
{
	return 0;
}

uint8_t predefined_set_cgi_processor(uint8_t * uri_name, uint8_t * uri, uint8_t * buf, uint16_t * len)
{
	return 0;
}


void task_http_data(void *parm)
{
	uint16_t rlen;
	
	httpServer_init(http_tbuf, http_rbuf, 1, http_sn);
	
	reg_httpServer_webContent("index.html", CONFIG_HTML);
	gen_w5500_js(&page_w5500_js, gWIZNETINFO);
	reg_httpServer_webContent("w5500.js", (uint8_t *)page_w5500_js);
	
	sem_http = xSemaphoreCreateBinary();
	
	while(1)
	{
		if(xSemaphoreTake(sem_http, portMAX_DELAY) == pdTRUE)
		{	
			if(SOCK_OK != getsockopt(SOCK_OF_HTTP, SO_RECVBUF, &rlen))
			{					
			}
//			if(rlen >= 8)
			{
				//rlen -= 8;
				httpServer_run(0);
				/*
				if(rlen > 0)
				{
					rbuf = pvPortMalloc(rlen + 1);
					if(rbuf != NULL)
					{
						ret = recv(SOCK_OF_HTTP, rbuf, rlen);
						if(ret == rlen)
						{
							*(rbuf + rlen) = 0;
							sendto(SOCK_OF_DEBUG, rbuf, rlen, dbg_ip, dbg_port);
							//proc_http(SOCK_OF_HTTP, rbuf);
							memcpy(http_rbuf, rbuf, rlen);
							
							//send(SOCK_OF_HTTP, (uint8_t *)CONFIG_HTML, strlen(CONFIG_HTML));
							//close(SOCK_OF_HTTP);
							//disconnect(SOCK_OF_HTTP);
							
							//parse_cmd((const char *)rbuf);
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
				}*/
			}
//			if(SOCK_OK != ctlsocket(SOCK_OF_HTTP, CS_CLR_INTERRUPT, &sik))
//			{
//			}

		}
	}
}


void task_http_conn(void *parm)
{
	uint8_t timecount;
	uint8_t so_status;
//	uint8_t ret;
//	uint8_t arg;
	

	
	
//	ctlwizchip(CW_GET_INTRTIME, &ret);
//	ctlsocket(SOCK_OF_HTTP, CS_GET_IOMODE, &ret);
//	ctlsocket(SOCK_OF_HTTP, CS_GET_INTMASK, &ret);
	
	//按如下方式设置中断MASK时，浏览器访问时，会产生CONNECT和RECIVED中断，否则产生的是CONNECT中断
//	arg = SIK_RECEIVED;
	//if(SOCK_OK != ctlsocket(SOCK_OF_HTTP, CS_SET_INTMASK, &arg))//
	{
		//arg = 7;
	}
	
	while(1)
	{
		if(SOCK_OK != getsockopt(SOCK_OF_HTTP, SO_STATUS, &so_status))
		{
		}
		switch(so_status)
		{
			case SOCK_CLOSE_WAIT:
				disconnect(SOCK_OF_HTTP);
				timecount = 0;
				break;
			
			case SOCK_CLOSED:
				if(socket(SOCK_OF_HTTP, Sn_MR_TCP, 80, 0x00) != SOCK_OF_HTTP)
				{
				}
				timecount = 0;
				break;
				
			case SOCK_INIT:
				listen(SOCK_OF_HTTP);
				timecount = 0;
				break;

			case SOCK_LISTEN:
				vTaskDelay(100);
				timecount = 0;
				break;
			
			case SOCK_ESTABLISHED:
				vTaskDelay(1000);
				timecount++;
				if(timecount >= 3)
				{
					disconnect(SOCK_OF_HTTP);
				}
				//close(SOCK_OF_HTTP);
				break;
			
			default :
				vTaskDelay(100);	
				timecount = 0;
				break;
		}
		
	}
}
