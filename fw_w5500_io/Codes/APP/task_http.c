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

char tmpstr[512];

uint8_t http_tbuf[2048];
uint8_t http_rbuf[2048];

uint8_t page_w5500_js[512];
uint8_t page_ajax_js[512];

uint8_t http_sn[1] = {SOCK_OF_HTTP};

uint8_t dbg_ip[4] = {192, 168, 1, 100};
uint16_t dbg_port = 9000;


uint32_t gen_w5500_js(uint8_t **content, void *parms)
{
	wiz_NetInfo *netInfo = parms;
	cJSON *jo = cJSON_CreateObject();
	uint32_t len;
	

//	if(*content != NULL)
//	{
//		vPortFree(content);
//	}
	
	cJSON_AddStringToObject(jo, "ver", "1.0");
	sprintf(tmpstr, "%02X:%02X:%02X:%02X:%02X:%02X",\
	netInfo->mac[0], netInfo->mac[1], netInfo->mac[2],\
	netInfo->mac[3], netInfo->mac[4], netInfo->mac[5]);	
	cJSON_AddStringToObject(jo, "mac", tmpstr);
	
	sprintf(tmpstr, "%d.%d.%d.%d",\
	netInfo->ip[0], netInfo->ip[1], netInfo->ip[2], netInfo->ip[3]);
	cJSON_AddStringToObject(jo, "ip", tmpstr);
	
	sprintf(tmpstr, "%d.%d.%d.%d",\
	netInfo->gw[0], netInfo->gw[1], netInfo->gw[2], netInfo->gw[3]);
	cJSON_AddStringToObject(jo, "gw", tmpstr);
	
	sprintf(tmpstr, "%d.%d.%d.%d",\
	netInfo->sn[0], netInfo->sn[1], netInfo->sn[2],netInfo->sn[3]);
	cJSON_AddStringToObject(jo, "sub", tmpstr);
	
	sprintf(tmpstr,	"settingsCallback(%s);",\
	cJSON_PrintUnformatted(jo));
	
	cJSON_Delete(jo);
	
	len = strlen(tmpstr);
	
	if(*content == NULL)
	{
		*content = page_w5500_js;
	}
	
//	*content = pvPortMalloc(len + 1);
//	if(*content != NULL)
	{
		memcpy(*content, tmpstr, len);
		*((*content) + len) = 0;
	}
  
	return len;
}

uint32_t gen_ajax_js(uint8_t **content, void *parms)
{
	uint32_t *systk = (uint32_t *)parms;
	uint32_t len;

//	if(*content != NULL)
//	{
//		vPortFree(content);
//	}
	
	sprintf(tmpstr, "%d",	*systk);		
	
	len = strlen(tmpstr);
	
	if(*content == NULL)
	{
		*content = page_ajax_js;
	}
	
//	*content = pvPortMalloc(len + 1);
//	if(*content != NULL)
	{
		memcpy(*content, tmpstr, len);
		*((*content) + len) = 0;
	}
//	else
//	{
//		return 0;
//	}
  
	return len;
}

uint8_t predefined_get_cgi_processor(uint8_t * uri_name, uint8_t * buf, uint16_t * len)
{
	return 0;
}

uint8_t predefined_set_cgi_processor(uint8_t * uri_name, uint8_t * uri, uint8_t * buf, uint16_t * len)
{
	return 0;
}

void task_http(void *parm)
{
//	uint8_t timecount;
//	uint8_t so_status;
	
	httpServer_init(http_tbuf, http_rbuf, 1, http_sn);
	
	reg_httpServer_webContent((uint8_t *)"config.html", (uint8_t *)CONFIG_HTML);		
	reg_httpServer_webContent((uint8_t *)"ajax.html", (uint8_t *)AJAX_HTML);
	
	reg_httpServer_webContent_dynamic((uint8_t *)"w5500.js", gen_w5500_js, &gWIZNETINFO);
	reg_httpServer_webContent_dynamic((uint8_t *)"ajax.js", gen_ajax_js, &sys_tick);

	
	sem_http = xSemaphoreCreateBinary();
	
	
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
		//xSemaphoreTake(sem_http, 100);
		if(pdTRUE == xSemaphoreTake(sem_http, 100))
		{
			httpServer_run(0);
		}
		else
			httpServer_run(0);
		/*
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
		*/
	}
}
