#ifndef MEW_M26_H
#define MEW_M26_H

#include "stdint.h"

typedef enum{
	M26_WS_IDLE,
	M26_WS_RESETING,
	M26_WS_GPRS_CONNECTING,	
	M26_WS_TCP_CONNECTING,
	M26_WS_SENDING,
	M26_WS_RECEIVING,
	M26_WS_RESET,
	M26_WS_GPRS_CONNECTED,
	M26_WS_TCP_CONNECTED,
	M26_WS_SENT,
	M26_WS_RECEIVED,
}mew_m26_WorkState_t;

typedef struct mew_m26_Handle_t
{
	
	char ADDR[6][64];
	char IP[6][15];
	uint16_t PORT[6];
	char IMEI[20];
	char Longitude[16];
	char Latitude[16];		
	
	uint16_t ConnectionInterval_Sec;
	uint32_t ReadInterval_MilliSec;
	uint16_t HeartbeatInterval_Sec;
	uint8_t EnableLoc;
	
	mew_m26_WorkState_t WorkState;
	
	uint8_t (*SocketStateGet)(uint8_t ch);		
	uint8_t (*SocketEnableGet)(uint8_t ch);	
	void (*SocketEnable)(uint8_t ch);
	void (*SocketDisable)(uint8_t ch);
	
	void (*SocketSend)(uint8_t ch, uint8_t *buff, uint16_t len);
	uint8_t (*HTTP_POST)(const char *url, uint16_t url_len, const uint8_t *payload, uint16_t payload_len, uint8_t *response);
	
	void (*Socket_Schedule_NoOS)(void);
	void (*HTTP_Schedule_NoOS)(void);
	
	uint8_t (*IsReceiving)(void);
}mew_m26_Handle_t;


extern mew_m26_Handle_t mew_m26;

void *mew_m26_Malloc_Hook(uint16_t size);
void mew_m26_Free_Hook(void *p);
void mew_m26_MallocErr_Hook(void);

void mew_m26_DelayMS_Hook(uint32_t timespan);
void mew_m26_SendBuff_Hook(uint8_t *buff, uint16_t len);

__weak void mew_m26_ResetStart_Hook(void);
__weak void mew_m26_Reset_Hook(void);
__weak void mew_m26_IdleHook(void);

__weak void mew_m26_GPRSConnDone_Hook(void);
__weak void mew_m26_GPRSConnErr_Hook(void);

__weak void mew_m26_SocketConnDone_Hook(uint8_t ch);
__weak void mew_m26_SocketConnErr_Hook(uint8_t ch, int8_t reason);
__weak void mew_m26_SocketDisconn_Hook(uint8_t ch, int8_t reason);

__weak void mew_m26_SocketHeartbeat_Hook(uint8_t ch);

__weak void mew_m26_SocketSendStart_Hook(uint8_t ch);
__weak void mew_m26_SocketSendDone_Hook(uint8_t ch);
__weak void mew_m26_SocketSendErr_Hook(uint8_t ch);

__weak void mew_m26_SocketRecvStart_Hook(uint8_t ch);
__weak void mew_m26_SocketRecvDone_Hook(uint8_t ch, uint8_t *buff, uint16_t len);
__weak void mew_m26_SocketRecvErr_Hook(uint8_t ch);
__weak void mew_m26_SocketRecvParseErr_Hook(uint8_t ch);

void mew_m26_Init(uint8_t *txbuff, uint16_t *txbufflen, uint8_t *rxbuff, uint16_t *rxbufflen, uint64_t *sticks);


#endif
