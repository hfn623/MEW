#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "stm32f10x.h"

#include "mew_board.h"
#include "mew_stm32.h"
#include "mew_m26.h"
#include "mew_nmea.h"

#include "cJSON.h"

#define FILTER_NUM 100

uint8_t login = 0;
uint8_t sending = 0;
int8_t located = 0;
uint64_t lastUpTicks;

cJSON_Hooks cjson_hooks;

static char tmpstr[1024];
static uint8_t tmpword[1024];

extern uint16_t gnss_rx_bufflen;
extern uint8_t gnss_rx_buff[GNSS_RX_BUFF_COUNT];
extern mew_GNGLL_Data gngll_data;
extern uint8_t gnss_frames;

void upLocInfo(void)
{
	char *tmpbuf;
	cJSON *upJSON = cJSON_CreateObject();
	
	cJSON_AddStringToObject(upJSON, "type", "DATA");	
	cJSON_AddNumberToObject(upJSON, "vaild", gngll_data.Vaild);	
	cJSON_AddNumberToObject(upJSON, "latitude", gngll_data.Latitude);
	cJSON_AddNumberToObject(upJSON, "longitude", gngll_data.Longitude);
	cJSON_AddNumberToObject(upJSON, "speed", 0);
	cJSON_AddNumberToObject(upJSON, "direction", 0);
	
	tmpbuf = cJSON_PrintUnformatted(upJSON);
	cJSON_Delete(upJSON);
	
	mew_m26.SocketSend(0, (uint8_t *)tmpbuf, strlen(tmpbuf));	
	
	free(tmpbuf);
}

static uint16_t find_max(uint16_t *list, uint16_t count)
{
	uint16_t i;
	uint16_t idx;
	uint16_t max;
	max = list[0];
	idx = 0;
	for(i = 0; i < count; i ++)
	{
		if(max < list[i])
		{
			max = list[i];
			idx = i;
		}
	}
	return idx;
}

static uint16_t find_min(uint16_t *list, uint16_t count)
{
	uint16_t i;
	uint16_t idx;
	uint16_t min;
	min = list[0];
	idx = 0;
	for(i = 0; i < count; i ++)
	{
		if(min > list[i])
		{
			min = list[i];
			idx = i;
		}
	}
	return idx;
}

static float getTempFromADC(void)
{
	uint16_t i = 0;
	uint16_t i_max;
	uint16_t i_min;
//	char str[16];
	uint16_t V_val[FILTER_NUM];
	float tmp = .0f;
	
	for(i = 0; i < FILTER_NUM; i ++)
	{
		V_val[i] = mew_stm32.ADCRead();
		mew_stm32.DelayMS(1);
	}
	
	i_max = find_max(V_val, FILTER_NUM);
	i_min = find_min(V_val, FILTER_NUM);
	
	for(i = 0; i < FILTER_NUM; i ++)
	{		
		tmp += V_val[i];
	}
	
	tmp -= V_val[i_max];
	tmp -= V_val[i_min];
	
	tmp /= FILTER_NUM - 2;
	
	tmp *= 3.3f;
	tmp /= 4096.0f;
	
	tmp -= 0.5f;
	
	tmp *= 100;
	
	return tmp;
}
void upTempInfo(void)
{
	char *tmpbuf;
	char tmpstr[32];
	cJSON *upJSON = cJSON_CreateObject();
	
	cJSON_AddStringToObject(upJSON, "type", "data");	
	cJSON_AddStringToObject(upJSON, "id", "02");	
	sprintf(tmpstr,"%.2f", getTempFromADC());
	cJSON_AddStringToObject(upJSON, "temp", tmpstr);
	
	tmpbuf = cJSON_PrintUnformatted(upJSON);
	cJSON_Delete(upJSON);
	
	mew_m26.SocketSend(0, (uint8_t *)tmpbuf, strlen(tmpbuf));	
	
	free(tmpbuf);
}

int main(void)
{
	cjson_hooks.malloc_fn = malloc;
  cjson_hooks.free_fn = free;
  cJSON_InitHooks(&cjson_hooks);
	
	mew_board_Init();
	
	sprintf(mew_m26.ADDR[0], "www.boryworks.com");
	mew_m26.PORT[0] = 9002;
	
	mew_m26.SocketEnable(0);
	
	mew_board.LED_NS(0);
	mew_board.MC20_PK(0);
	
	mew_board.LED_TX(0);
	mew_board.LED_RX(0);
	mew_board.LED_STA(0);
	
	while(1)
	{
		mew_m26.Socket_Schedule_NoOS();
		
		/*
		if(gnss_frames > 0)
		{
			located = mew_NMEA_Parse_GNGLL((char *)gnss_rx_buff, &gngll_data);
			if(0 == located)
			{
				mew_board.LED_STA(0);				
			}
			else
			{
				mew_board.LED_STA(1);
			}
			gnss_rx_bufflen = 0;
			gnss_frames = 0;
		}
		if(!sending && login && !located)
		{
			if(mew_stm32.Nowticks - lastUpTicks > 1000 * 10)
			{
				lastUpTicks = mew_stm32.Nowticks;
				if(login)
					upLocInfo();
			}
		}
		*/
		if(mew_stm32.Nowticks - lastUpTicks > 1000 * 60)
		{
			lastUpTicks = mew_stm32.Nowticks;			
			upTempInfo();
		}
	}
}

void mew_m26_Reset_Hook(void)
{
	mew_board.LED_TX(1);
	mew_board.LED_RX(1);
	mew_board.LED_STA(1);
	mew_board.LED_NS(0);
}

void mew_m26_GPRSConnDone_Hook(void)
{
	mew_board.LED_NS(1);	
}

void mew_m26_SocketConnDone_Hook(uint8_t ch)
{
	login = 0;
	
	mew_board.LED_TX(0);
	mew_board.LED_RX(0);
	
	sprintf(tmpstr, "lib event: socket%d conn done\n", ch);
	mew_stm32.UARTSendString(1, tmpstr);
}
void mew_m26_SocketConnErr_Hook(uint8_t ch, int8_t reason)
{
	mew_board.LED_TX(1);
	mew_board.LED_RX(1);
	
	sprintf(tmpstr, "lib event: socket%d conn err, reason = %d\n", ch, reason);
	mew_stm32.UARTSendString(1, tmpstr);
}

void mew_m26_SocketSendStart_Hook(uint8_t ch)
{
	sending = 1;
	
	mew_board.LED_TX(1);
	sprintf(tmpstr, "lib event: socket%d send start\n", ch);
	mew_stm32.UARTSendString(1, tmpstr);
}
void mew_m26_SocketSendDone_Hook(uint8_t ch)
{
	sending = 0;
	
	mew_board.LED_TX(0);
	sprintf(tmpstr, "lib event: socket%d send done\n", ch);
	mew_stm32.UARTSendString(1, tmpstr);
}
void mew_m26_SocketSendErr_Hook(uint8_t ch)
{
	mew_board.LED_TX(1);
	mew_board.LED_RX(1);	
	sprintf(tmpstr, "lib event: socket%d send err\n", ch);
	mew_stm32.UARTSendString(1, tmpstr);
}

void mew_m26_SocketHeartbeat_Hook(uint8_t ch)
{
	char *tmpbuf;
	cJSON *upJSON = cJSON_CreateObject();
	
	cJSON_AddStringToObject(upJSON, "type", "HEARTBEAT");	
	
	tmpbuf = cJSON_PrintUnformatted(upJSON);
	cJSON_Delete(upJSON);
	
	mew_m26.SocketSend(ch, (uint8_t *)tmpbuf, strlen(tmpbuf));	
	
	free(tmpbuf);
}
void mew_m26_SocketRecvStart_Hook(uint8_t ch)
{
	mew_board.LED_RX(1);
	
	sprintf(tmpstr, "lib event: socket%d recv start\n", ch);
	mew_stm32.UARTSendString(1, tmpstr);
}

void mew_m26_SocketRecvDone_Hook(uint8_t ch, uint8_t *buff, uint16_t len)
{	
	cJSON *JSONreq, *JSONresp;
	cJSON *type;
	uint8_t *start_idx, *end_idx, *buffbak;
	
	char *tmpbuf;
	uint16_t tmplen;
	
	
	mew_board.LED_RX(0);
	
	sprintf(tmpstr, "lib event: socket%d recv done\n", ch);
	mew_stm32.UARTSendString(1, tmpstr);
	
	while(1)
	{
		start_idx = (uint8_t *)strstr((char *)buff, "{");
		end_idx = (uint8_t *)strstr((char *)buff, "}");
		
		if(start_idx != NULL && end_idx != NULL)
		{
			
			tmplen = end_idx - start_idx + 1;
			memcpy(tmpword, start_idx, tmplen);
			tmpword[tmplen] = 0;
			
			
			JSONreq = cJSON_Parse((char const *)tmpword);			
			type =  cJSON_GetObjectItem(JSONreq, "type");
			
			if(!strcmp(type->valuestring, "auth"))
			{				
				JSONresp = cJSON_CreateObject();
				
				cJSON_AddStringToObject(JSONresp, "type", "mc20_db_lts");	
				
				tmpbuf = cJSON_PrintUnformatted(JSONresp);
				cJSON_Delete(JSONresp);
				
				mew_m26.SocketSend(ch, (uint8_t *)tmpbuf, strlen(tmpbuf));	
				
				free(tmpbuf);
				
				
			}
			else if(!strcmp(type->valuestring, "allow"))
			{
				login = 1;
			}
			cJSON_Delete(JSONreq);
			if((end_idx - buffbak) < len - 1)
			{
				buff = end_idx + 1;
			}
			else
			{
				break;
			}
		}
		else	
		{
			break;
		}					
	}
}
void mew_m26_SocketRecvErr_Hook(uint8_t ch)
{
	mew_board.LED_TX(1);
	mew_board.LED_RX(1);
	
	sprintf(tmpstr, "lib event: socket%d recv err\n", ch);
	mew_stm32.UARTSendString(1, tmpstr);
}
void mew_m26_SocketDisconn_Hook(uint8_t ch, int8_t reason)
{
	login = 0;
	
	mew_board.LED_RX(1);
	mew_board.LED_TX(1);
	
	mew_board.LED_NS(0);
	
	sprintf(tmpstr, "lib event: socket%d disconn, reason = %d\n", ch, reason);
	mew_stm32.UARTSendString(1, tmpstr);
}
