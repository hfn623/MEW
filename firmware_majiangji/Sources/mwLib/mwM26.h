#ifndef MW_M26_H
#define MW_M26_H

#include "stdint.h"


#define AT_CMD_TIMEOUT			300
#define CHANNEL_COUNT				6

extern uint8_t reading_cmd;

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
}mwM26WorkState_t;

typedef struct mwM26Handle_t{
	char ADDR[6][64];
	char IP[6][15];
	uint16_t PORT[6];
	char IMEI[20];
	char Longitude[16];
	char Latitude[16];
	mwM26WorkState_t WorkState;
	uint32_t HeartbeatInterval_Sec;
	uint32_t ConnectionInterval_Sec;
	uint32_t ReadInterval_MilliSec;
	
	uint8_t (*SocketStateGet)(uint8_t ch);	
	
	uint8_t (*SocketEnableGet)(uint8_t ch);
	
	void (*SocketEnable)(uint8_t ch);
	void (*SocketDisable)(uint8_t ch);
	
	void (*Schedule_Socket_NoOS)(void);
	void (*Schedule_HTTP_NoOS)(void);
	
	void (*SocketSend)(uint8_t ch, uint8_t *buff, uint16_t len);
	uint8_t (*HTTP_POST)(const char *url, uint16_t url_len, const uint8_t *payload, uint16_t payload_len, uint8_t *response);
	uint8_t EnableLoc;
	

}mwM26Handle_t;

extern mwM26Handle_t mwM26;


__weak void *mwM26MallocHook(uint16_t size);
__weak void mwM26FreeHook(void *p);
__weak void mwM26DelayMSHook(uint32_t timespan);
__weak void mwM26SendHook(uint8_t *buff, uint16_t len);

__weak void mwM26IdleHook(void);

__weak void mwM26SocketSendStartHook(uint8_t ch);
__weak void mwM26SocketSendDoneHook(uint8_t ch);
__weak void mwM26SocketSendErrHook(uint8_t ch);

__weak void mwM26SocketRecvStartHook(uint8_t ch);
__weak void mwM26SocketRecvDoneHook(uint8_t ch, uint8_t *buff, uint16_t len);

__weak void mwM26MallocErrHook(void);
__weak void mwM26ParseErrHook(void);
__weak void mwM26SocketConnDoneHook(uint8_t ch);
__weak void mwM26SocketConnErrHook(uint8_t ch, int8_t reason);
__weak void mwM26GPRSConnDoneHook(void);
__weak void mwM26GPRSConnErrHook(void);

__weak void mwM26ResetStartHook(void);

void mwM26Init(uint8_t *txbuff, uint16_t *txbufflen, uint8_t *rxbuff, uint16_t *rxbufflen, uint64_t *sticks);

#endif
