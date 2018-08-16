#include "mew_board.h"
#include "mew_stm32.h"
#include "mew_m26.h"
#include "stm32f10x.h"
#include "stdio.h"

#include "stdlib.h"
#include "string.h"

#include "cJSON.h"

#define GPRS_TX_BUFF_COUNT 2048
#define GPRS_RX_BUFF_COUNT 2048
#define GNSS_RX_BUFF_COUNT 2048

static uint8_t debug = 1;

static uint8_t relay_stat;

static cJSON_Hooks cjson_hooks;

mew_board_Handle_t mew_board;

static uint8_t gprs_tx_buff[GPRS_TX_BUFF_COUNT];
static uint8_t gprs_rx_buff[GPRS_RX_BUFF_COUNT];

static uint16_t gprs_tx_bufflen;
static uint16_t gprs_rx_bufflen;

static char tmpstr[512];

void mew_stm32_PINsInit_Hook(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// mcu io
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 

	// wkup
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 

	// m26 pk
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 	
	
	// m26 net
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

	//
	
	// led tx
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  

	// led rx
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  

	// led sta
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
}
static void led_tx(uint8_t state)
{
	mew_stm32.PortBWrite(0, state);
}
static void led_ns(uint8_t state)
{
	mew_stm32.PortBWrite(9, state);
}
static void led_rx(uint8_t state)
{
	mew_stm32.PortAWrite(7, state);
}
static void led_sta(uint8_t state)
{
	mew_stm32.PortAWrite(6, state);
}
static void pin_io(uint8_t state)
{
	mew_stm32.PortAWrite(3, state);
}
static void pin_wkup(uint8_t state)
{
	mew_stm32.PortAWrite(0, state);
}
static void m26_pk(uint8_t state)
{
	mew_stm32.PortAWrite(8, state);
}

void Relay_Pos_Toggle(uint8_t set)
{
	mew_board.PIN_IO(set);
}

void Relay_Neg_Toggle(uint8_t set)
{
  mew_board.PIN_WKUP(set);
}

void mew_board_Init(void)
{
	mew_board.LED_NS = led_ns;
	mew_board.LED_RX = led_rx;
	mew_board.LED_TX = led_tx;
	mew_board.LED_STA = led_sta;
	mew_board.PIN_IO = pin_io;
	mew_board.PIN_WKUP = pin_wkup;
	mew_board.M26_PK = m26_pk;
	
	mew_stm32_Init(115200, 115200, 115200);	
			
	mew_m26_Init(
	gprs_tx_buff, &gprs_tx_bufflen,
	gprs_rx_buff, &gprs_rx_bufflen,
	&mew_stm32.Nowticks);
	
	sprintf(mew_m26.ADDR[1], "www.magiceworks.com");
	mew_m26.PORT[1] = 9000;
	
	sprintf(mew_m26.ADDR[0], "api.jimagj.com");
	mew_m26.PORT[0] = 1235;
	
	mew_m26.SocketEnable(0);
//	mew_m26.SocketEnable(1);
	
	mew_board.LED_NS(1);	
	
	mew_board.M26_PK(0);
	
  cjson_hooks.malloc_fn = malloc;
  cjson_hooks.free_fn = free;
  cJSON_InitHooks(&cjson_hooks);
	
	relay_stat = 0;
	Relay_Neg_Toggle(1);
	mew_stm32.DelayMS(100);
	Relay_Neg_Toggle(0);
	
//	sprintf(tmpstr, "user event: board init\n");
//	mew_stm32.UARTSendString(3, tmpstr);
}
////////////////////////////////////////////////////////////////
//板级抽象钩子函数
////////////////////////////////////////////////////////////////
void mew_m26_DelayMS_Hook(uint32_t timespan)
{
	mew_stm32.DelayMS(timespan);
}
void mew_m26_SendBuff_Hook(uint8_t *buff, uint16_t len)
{
	uint16_t i;
	for(i= 0; i< len; i++)
	{
		mew_stm32.UARTSendByte(1, buff[i]);
	}
}
//void mew_stm32_UARTRecvDone_Hook(uint8_t port)
//{
////	uint16_t i;
//	if(port == 2)
//	{
//		//缓存内容全部发给用户
////		for(i = 0; i < gnss_rx_bufflen; i ++)
////		{
////			mew_stm32.UARTSendByte(1, gnss_rx_buff[i]);
////		}
//		gnss_rx_bufflen = 0;
//	}
//}
void mew_stm32_UARTRecvByte_Hook(uint8_t port, uint8_t byte)
{
	//串口3是用户串口
	if(port == 3)
	{
		//直接转发给GPRS部分
		mew_stm32.UARTSendByte(1, byte);
	}
	//串口3连接GPRS模块
	if(port == 1)
	{
		if(gprs_rx_bufflen < GPRS_RX_BUFF_COUNT)
		{
			gprs_rx_buff[gprs_rx_bufflen ++]= byte;
		}
//		if(debug && mew_m26.IsReceiving() == 0)
//		{
//			mew_stm32.UARTSendByte(3, byte);
//		}
	}
}
////////////////////////////////////////////////////////////////
//用户级抽象钩子函数
////////////////////////////////////////////////////////////////
void mew_m26_GPRSConnDone_Hook(void)
{
	mew_board.LED_STA(0);
	mew_board.LED_TX(0);
	mew_board.LED_RX(0);
	
//	mew_stm32.UARTSendString(3, "lib event: gprs conn done\n");
}

void mew_m26_GPRSConnErr_Hook(void)
{
	mew_board.LED_STA(1);
	mew_board.LED_TX(1);
	mew_board.LED_RX(1);
	NVIC_SystemReset();
	
//	mew_stm32.UARTSendString(3, "lib event: gprs conn err\n");
}

void mew_m26_ResetStart_Hook(void)
{
	mew_board.LED_STA(0);
	mew_board.LED_TX(1);
	mew_board.LED_RX(1);
	
//	mew_stm32.UARTSendString(3, "lib event: gprs reset\n");
}


void mew_m26_SocketSendStart_Hook(uint8_t ch)
{
	mew_board.LED_TX(1);
	
//	sprintf(tmpstr, "lib event: socket%d send start\n", ch);
//	mew_stm32.UARTSendString(3, tmpstr);
}
void mew_m26_SocketSendDone_Hook(uint8_t ch)
{
	mew_board.LED_TX(0);
	
//	sprintf(tmpstr, "lib event: socket%d send done\n", ch);
//	mew_stm32.UARTSendString(3, tmpstr);
}
void mew_m26_SocketSendErr_Hook(uint8_t ch)
{
	mew_board.LED_STA(1);
	mew_board.LED_TX(1);
	mew_board.LED_RX(1);
	
//	sprintf(tmpstr, "lib event: socket%d send err\n", ch);
//	mew_stm32.UARTSendString(3, tmpstr);
}
void mew_m26_SocketHeartbeat_Hook(uint8_t ch)
{
	char *tmpbuf;
	uint16_t tmplen;
	cJSON *JSONresp;
	
	JSONresp = cJSON_CreateObject();
	cJSON_AddStringToObject(JSONresp, "order", "*");
	cJSON_AddStringToObject(JSONresp, "desk_id", mew_m26.IMEI);
	cJSON_AddStringToObject(JSONresp, "st", relay_stat ? "1" : "0");

	tmpbuf = cJSON_PrintUnformatted(JSONresp);
	
	tmplen = strlen((const char *)tmpbuf);
	
	mew_m26.SocketSend(ch, (uint8_t *)tmpbuf, tmplen);
	mew_stm32.UARTSendString(3, tmpbuf);
	
	cJSON_Delete(JSONresp);
	
	free(tmpbuf);
	
//	sprintf(tmpstr, "\nuser event: heardbeat packet upload\n");
//	mew_stm32.UARTSendString(3, tmpstr);
}
void mew_m26_SocketRecvStart_Hook(uint8_t ch)
{
	mew_board.LED_RX(1);
	
//	sprintf(tmpstr, "lib event: socket%d recv start\n", ch);
//	mew_stm32.UARTSendString(3, tmpstr);
}
static uint8_t tmpword[1024];
void mew_m26_SocketRecvDone_Hook(uint8_t ch, uint8_t *buff, uint16_t len)
{	
	uint16_t i;	
	char *tmpbuf;
	uint16_t tmplen;
	
	uint8_t *start_idx, *end_idx, *buffbak;

	
//	mew_buff_Handle_t bh;
//	mew_buff_Handle_t stag, etag;
	
	cJSON *JSONreq;
  cJSON *JSONresp;
	
  cJSON *order, *type;

	mew_board.LED_RX(0);
	
//	sprintf(tmpstr, "lib event: socket%d recv done\n", ch);
//	mew_stm32.UARTSendString(3, tmpstr);
	
//	bh.pBuff = buff;
//	bh.Length = len;
//	
//	stag.pBuff = "{";
//	str

	buffbak = buff;
	
	while(1)
	{
		start_idx = (uint8_t *)strstr((char *)buff, "{");
		end_idx = (uint8_t *)strstr((char *)buff, "}");
		
		if(start_idx != NULL && end_idx != NULL)
		{
			
			tmplen = end_idx - start_idx + 1;
			memcpy(tmpword, start_idx, tmplen);
			tmpword[tmplen] = 0;
		
//			mew_stm32.UARTSendString(3, "\nhandle json pack:\n");
		
//			mew_stm32.UARTSendByte(3, '\n');
//			for(i = 0; i < tmplen; i ++)
//			{
//				mew_stm32.UARTSendByte(3, '=');
//			}	
//			mew_stm32.UARTSendByte(3, '\n');
//			
//			mew_stm32.UARTSendString(3, (char *)tmpword);
//			
//			mew_stm32.UARTSendByte(3, '\n');
//			for(i = 0; i < tmplen; i ++)
//			{
//				mew_stm32.UARTSendByte(3, '=');
//			}
//			mew_stm32.UARTSendByte(3, '\n');

			
			JSONreq = cJSON_Parse((char const *)tmpword);
			order = cJSON_GetObjectItem(JSONreq, "order");
			type =  cJSON_GetObjectItem(JSONreq, "type");
			if(order && type)
			{
				if(!strcmp(order->valuestring, "init") && !strcmp(type->valuestring, "send"))
				{
					mew_board.LED_STA(0);
					
					JSONresp = cJSON_CreateObject();
					cJSON_AddStringToObject(JSONresp, "desk_id", mew_m26.IMEI);
					cJSON_AddStringToObject(JSONresp, "ip", "0.0.0.0");
					cJSON_AddStringToObject(JSONresp, "mac", "00-00-00-00-00-00");
					cJSON_AddNumberToObject(JSONresp, "status", 0);
					cJSON_AddStringToObject(JSONresp, "longitude", "-18.3");
					cJSON_AddStringToObject(JSONresp, "latitude", "65.93");
					cJSON_AddStringToObject(JSONresp, "order", "init");
					cJSON_AddNumberToObject(JSONresp, "code", 200);
					tmpbuf = cJSON_PrintUnformatted(JSONresp);
					
					tmplen = strlen((const char *)tmpbuf);
					
					mew_m26.SocketSend(ch, (uint8_t *)tmpbuf, tmplen);
					
//					mew_stm32.UARTSendString(3, tmpbuf);
					
					cJSON_Delete(JSONresp);
					
					free(tmpbuf);
					
//					sprintf(tmpstr, "\nuser event: server init\n");
//					mew_stm32.UARTSendString(3, tmpstr);
				}
				if(!strcmp(order->valuestring, "init") && !strcmp(type->valuestring, "reply"))
				{ 
					mew_board.LED_STA(1);
					
//					sprintf(tmpstr, "\nuser event: server init done\n");
//					mew_stm32.UARTSendString(3, tmpstr);
				}
				if(!strcmp(order->valuestring, "turn_on") && !strcmp(type->valuestring, "send"))
				{
					mew_board.LED_STA(0);
					
					JSONresp = cJSON_CreateObject();
					cJSON_AddStringToObject(JSONresp, "desk_id", mew_m26.IMEI);
					cJSON_AddNumberToObject(JSONresp, "status", 1);
					cJSON_AddStringToObject(JSONresp, "longitude", "-18.3");
					cJSON_AddStringToObject(JSONresp, "latitude", "65.93");
					cJSON_AddStringToObject(JSONresp, "order", "turn_on");
					cJSON_AddStringToObject(JSONresp, "code", "200");
					tmpbuf = cJSON_PrintUnformatted(JSONresp);
					
					tmplen = strlen((const char *)tmpbuf);
								
					
					relay_stat = 1;
					Relay_Pos_Toggle(1);
					mew_stm32.DelayMS(100);
					Relay_Pos_Toggle(0);
					
					mew_m26.SocketSend(ch, (uint8_t *)tmpbuf, tmplen);
					
//					mew_stm32.UARTSendString(3, tmpbuf);
					
					cJSON_Delete(JSONresp);
					
					free(tmpbuf);
					
//					sprintf(tmpstr, "\nuser event: server turn on\n");
//					mew_stm32.UARTSendString(3, tmpstr);
				}
				if(!strcmp(order->valuestring, "turn_on") && !strcmp(type->valuestring, "reply"))
				{
					mew_board.LED_STA(1);
					
//					sprintf(tmpstr, "user event: server turn on done\n");
//					mew_stm32.UARTSendString(3, tmpstr);
				}
				if(!strcmp(order->valuestring, "turn_off") && !strcmp(type->valuestring, "send"))
				{
					mew_board.LED_STA(0);
					
					JSONresp = cJSON_CreateObject();
					cJSON_AddStringToObject(JSONresp, "desk_id", mew_m26.IMEI);
					cJSON_AddNumberToObject(JSONresp, "status", 1);
					cJSON_AddStringToObject(JSONresp, "longitude", "-18.3");
					cJSON_AddStringToObject(JSONresp, "latitude", "65.93");
					cJSON_AddStringToObject(JSONresp, "order", "turn_off");
					cJSON_AddStringToObject(JSONresp, "code", "200");
					tmpbuf = cJSON_PrintUnformatted(JSONresp);
					
					tmplen = strlen((const char *)tmpbuf);
					
					
					relay_stat = 0;
					Relay_Neg_Toggle(1);
					mew_stm32.DelayMS(100);
					Relay_Neg_Toggle(0);
					
					mew_m26.SocketSend(ch, (uint8_t *)tmpbuf, tmplen);
					
//					mew_stm32.UARTSendString(3, tmpbuf);
					
					cJSON_Delete(JSONresp);
					
					free(tmpbuf);
					
//					sprintf(tmpstr, "\nuser event: server turn off\n");
//					mew_stm32.UARTSendString(3, tmpstr);
				}
				if(!strcmp(order->valuestring, "turn_off") && !strcmp(type->valuestring, "reply"))
				{
					mew_board.LED_STA(1);
					
//					sprintf(tmpstr, "user event: server turn off done\n");
//					mew_stm32.UARTSendString(3, tmpstr);
				}
			}
			cJSON_Delete(JSONreq);
		}
		else
		{
//			sprintf(tmpstr, "user event: recv invaild json pack\n");
//			mew_stm32.UARTSendString(3, tmpstr);
			break;
		}
		
		
		if((end_idx - buffbak) < len - 1)
		{
			buff = end_idx + 1;
		}
		else
		{
			break;
		}
	}
}
void mew_m26_SocketRecvErr_Hook(uint8_t ch)
{
	mew_board.LED_RX(1);
	
//	sprintf(tmpstr, "lib event: socket%d recv err\n", ch);
//	mew_stm32.UARTSendString(3, tmpstr);
}
void mew_m26_SocketDisconn_Hook(uint8_t ch, int8_t reason)
{
	mew_board.LED_RX(1);
	mew_board.LED_TX(1);
	mew_board.LED_STA(0);
	
//	sprintf(tmpstr, "lib event: socket%d disconn, reason = %d\n", ch, reason);
//	mew_stm32.UARTSendString(3, tmpstr);
}
