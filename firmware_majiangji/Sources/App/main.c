
#include "stm32f10x.h"
#include "mwSTM32.h"
#include "mwM26.h"
#include "stdio.h"
#include "cJSON.h"
#include "stdlib.h"
#include "string.h"

#define UART_RBUF_SIZE 512

uint16_t stm32uart1rbuf_len;
uint8_t stm32uart1rbuf[UART_RBUF_SIZE];

uint16_t stm32uart1tbuf_len;
uint8_t stm32uart1tbuf[UART_RBUF_SIZE];

uint16_t stm32uart3rbuf_len;
uint8_t stm32uart3rbuf[UART_RBUF_SIZE];

void Relay_Pos_Toggle(uint8_t set)
{
  if(set)
  {
    GPIO_SetBits(GPIOB, GPIO_Pin_1);
  }
  else
  {
    GPIO_ResetBits(GPIOB, GPIO_Pin_1);
  }
}

void Relay_Neg_Toggle(uint8_t set)
{
  if(set)
  {
    GPIO_SetBits(GPIOA, GPIO_Pin_3);
  }
  else
  {
    GPIO_ResetBits(GPIOA, GPIO_Pin_3);
  }
}

void mwM26GPRSConnDoneHook(void)
{
	mwSTM32.PinLEDSTA(0);
	mwSTM32.PinLEDTXD(0);
	mwSTM32.PinLEDRXD(0);
}
void mwM26GPRSConnErrHook(void)
{
	mwSTM32.PinLEDSTA(1);
	mwSTM32.PinLEDTXD(1);
	mwSTM32.PinLEDRXD(1);
	NVIC_SystemReset();
}
void mwM26ResetStartHook(void)
{
	mwSTM32.PinLEDSTA(0);
	mwSTM32.PinLEDTXD(1);
	mwSTM32.PinLEDRXD(1);
}

void mwSTM32UARTRecvByteHook(uint8_t port, uint8_t byte)
{
	if(port == 1)
	{
		if(stm32uart1rbuf_len < UART_RBUF_SIZE)
		{
			stm32uart1rbuf[stm32uart1rbuf_len ++] = byte;
		}
		else
		{
			stm32uart1rbuf_len = 0;
		}			
		
		if(reading_cmd == 0)
		{
			mwSTM32.UARTSendByte(3, byte);
		}
	}
	if(port == 3)
	{
		stm32uart3rbuf[stm32uart3rbuf_len ++] = byte;
	}
}

void mwSTM32UARTRecvDoneHook(uint8_t port)
{
//	if(port == 3)//Êý¾ÝÍ¸´«
//	{
//		mwM26.SocketSend(0, stm32uart3rbuf, stm32uart3rbuf_len);
//		stm32uart3rbuf_len = 0;
//	}
}

void mwM26SocketRecvStartHook(uint8_t ch)
{
	mwSTM32.PinLEDRXD(1);
}

void mwM26SocketSendStartHook(uint8_t ch)
{
	mwSTM32.PinLEDTXD(1);
}
void mwM26SocketSendDoneHook(uint8_t ch)
{
	mwSTM32.PinLEDTXD(0);
}
void mwM26SocketSendErrHook(uint8_t ch)
{
	mwSTM32.PinLEDSTA(0);
	mwSTM32.PinLEDTXD(1);
	mwSTM32.PinLEDRXD(1);
}

void mwM26SocketRecvDoneHook(uint8_t ch, uint8_t *buff, uint16_t len)
{
	uint16_t i;
//	for(i = 0; i < len; i ++)
//	{
//		mwSTM32.UARTSendByte(3, buff[i]);
//	}
	
	char *tmpbuf;
//	BW_PackHandle_t spack;
	uint16_t tmplen;
	
	cJSON *JSONreq;
  cJSON *JSONresp;
	
  cJSON *order, *type;
	
//	UART1Send(pack.hBuff, pack.Length);
//	UART1Send("\n", 1);
	mwSTM32.PinLEDRXD(0);
	
	//if(ch == 0)
	{
		
		mwSTM32.UARTSendByte(3, '\n');
		for(i = 0; i < len; i ++)
		{
			mwSTM32.UARTSendByte(3, '=');
		}	
		mwSTM32.UARTSendByte(3, '\n');
		
		for(i = 0; i < len; i ++)
		{
			mwSTM32.UARTSendByte(3, buff[i]);
		}		
		
		mwSTM32.UARTSendByte(3, '\n');
		for(i = 0; i < len; i ++)
		{
			mwSTM32.UARTSendByte(3, '=');
		}
		mwSTM32.UARTSendByte(3, '\n');

		
		JSONreq = cJSON_Parse((char const *)buff);
		order = cJSON_GetObjectItem(JSONreq, "order");
		type =  cJSON_GetObjectItem(JSONreq, "type");
		if(order && type)
		{
			if(!strcmp(order->valuestring, "init") && !strcmp(type->valuestring, "send"))
			{
				JSONresp = cJSON_CreateObject();
				cJSON_AddStringToObject(JSONresp, "desk_id", mwM26.IMEI);
				cJSON_AddStringToObject(JSONresp, "ip", "0.0.0.0");
				cJSON_AddStringToObject(JSONresp, "mac", "00-00-00-00-00-00");
				cJSON_AddNumberToObject(JSONresp, "status", 0);
				cJSON_AddStringToObject(JSONresp, "longitude", "-18.3");
				cJSON_AddStringToObject(JSONresp, "latitude", "65.93");
				cJSON_AddStringToObject(JSONresp, "order", "init");
				cJSON_AddNumberToObject(JSONresp, "code", 200);
				tmpbuf = cJSON_PrintUnformatted(JSONresp);
				
				//spack.hBuff = (uint8_t *)tmpbuf;
				tmplen = strlen((const char *)tmpbuf);
				
				mwM26.SocketSend(ch, (uint8_t *)tmpbuf, tmplen);
	//			BW_M26.SendPayload(ch, spack);
				
				cJSON_Delete(JSONresp);
				
				free(tmpbuf);
				
				mwSTM32.PinLEDSTA(1);
			}
			if(!strcmp(order->valuestring, "init") && !strcmp(type->valuestring, "reply"))
			{  
	//			UART1Send("\n===init reply===\n", 18);
			}
			if(!strcmp(order->valuestring, "turn_on") && !strcmp(type->valuestring, "send"))
			{
				JSONresp = cJSON_CreateObject();
				cJSON_AddStringToObject(JSONresp, "desk_id", mwM26.IMEI);
				cJSON_AddNumberToObject(JSONresp, "status", 1);
				cJSON_AddStringToObject(JSONresp, "longitude", "-18.3");
				cJSON_AddStringToObject(JSONresp, "latitude", "65.93");
				cJSON_AddStringToObject(JSONresp, "order", "turn_on");
				cJSON_AddStringToObject(JSONresp, "code", "200");
				tmpbuf = cJSON_PrintUnformatted(JSONresp);
				
	//			spack.hBuff = (uint8_t *)tmpbuf;
				tmplen = strlen((const char *)tmpbuf);
							
				
				Relay_Pos_Toggle(1);
				mwSTM32.DelayMS(100);
				Relay_Pos_Toggle(0);
				
				mwM26.SocketSend(ch, (uint8_t *)tmpbuf, tmplen);
	//			BW_M26.SendPayload(ch, spack);
				
				cJSON_Delete(JSONresp);
				
				free(tmpbuf);
			}
			if(!strcmp(order->valuestring, "turn_on") && !strcmp(type->valuestring, "reply"))
			{  
	//			UART1Send("\n===turn_on reply===\n", 21);
			}
			if(!strcmp(order->valuestring, "turn_off") && !strcmp(type->valuestring, "send"))
			{
				JSONresp = cJSON_CreateObject();
				cJSON_AddStringToObject(JSONresp, "desk_id", mwM26.IMEI);
				cJSON_AddNumberToObject(JSONresp, "status", 1);
				cJSON_AddStringToObject(JSONresp, "longitude", "-18.3");
				cJSON_AddStringToObject(JSONresp, "latitude", "65.93");
				cJSON_AddStringToObject(JSONresp, "order", "turn_off");
				cJSON_AddStringToObject(JSONresp, "code", "200");
				tmpbuf = cJSON_PrintUnformatted(JSONresp);
				
	//			spack.hBuff = (uint8_t *)tmpbuf;
				tmplen = strlen((const char *)tmpbuf);
				
				Relay_Neg_Toggle(1);
				mwSTM32.DelayMS(100);
				Relay_Neg_Toggle(0);
				
				mwM26.SocketSend(ch, (uint8_t *)tmpbuf, tmplen);
	//			BW_M26.SendPayload(ch, spack);
				
				cJSON_Delete(JSONresp);
				
				free(tmpbuf);
			}
			if(!strcmp(order->valuestring, "turn_off") && !strcmp(type->valuestring, "reply"))
			{  
	//			UART1Send("\n===turn_off reply===\n", 22);
			}
		}
		
		
		cJSON_Delete(JSONreq);
	}
	
}

void mwM26SendHook(uint8_t *buff, uint16_t len)
{
	uint16_t i;
	for(i = 0; i < len; i ++)
	{
		mwSTM32.UARTSendByte(1, buff[i]);
	}
}

void mwM26DelayMSHook(uint32_t timespan)
{
	mwSTM32.DelayMS(timespan);
}

int main(void)
{
	cJSON_Hooks cjson_hooks;
  cjson_hooks.malloc_fn = malloc;
  cjson_hooks.free_fn = free;
  cJSON_InitHooks(&cjson_hooks);
	
	mwSTM32Init();
	mwM26Init(stm32uart1tbuf, &stm32uart1tbuf_len, stm32uart1rbuf, &stm32uart1rbuf_len, &mwSTM32.Nowticks);
	
	sprintf(mwM26.ADDR[1], "www.magiceworks.com");
	mwM26.PORT[1] = 9000;
	
	sprintf(mwM26.ADDR[0], "api.jimagj.com");
	mwM26.PORT[0] = 1235;
	
	mwM26.SocketEnable(0);
	mwM26.SocketEnable(1);
	
	mwSTM32.PinM26NET(1);
	mwSTM32.PinM26PK(0);	
	
	Relay_Neg_Toggle(1);
	mwSTM32.DelayMS(100);
	Relay_Neg_Toggle(0);
	
	while(1)
	{
			Relay_Neg_Toggle(1);
			mwSTM32.DelayMS(3000);
			Relay_Neg_Toggle(0);			
		mwSTM32.DelayMS(3000);
		
			Relay_Pos_Toggle(1);
			mwSTM32.DelayMS(3000);
			Relay_Pos_Toggle(0);		
		mwSTM32.DelayMS(3000);
		
//		mwM26.Schedule_Socket_NoOS();		
	}
}
