#ifndef MW_M26_H
#define MW_M26_H

#include "stdint.h"
#include "mwbuff.h"

#define AT_CMD_TIMEOUT			300
#define CHANNEL_COUNT				6

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
	uint16_t PORT[6];
	char IMEI[20];
	char Longitude[16];
	char Latitude[16];
	mwM26WorkState_t WorkState;
	uint32_t HeartbeatInterval_Sec;
	uint32_t ConnectionInterval_Sec;
	uint32_t ReadInterval_MilliSec;
	uint8_t (*TCPStateGet)(uint8_t ch);
	
	uint8_t (*TCPEnableGet)(uint8_t ch);
	void (*TCPEnableSet)(uint8_t ch);
	void (*TCPEnableClr)(uint8_t ch);
	
	void (*Schedule_TCP_NoOS)(void);
	void (*Schedule_HTTP_NoOS)(void);
	
	uint8_t (*SendPayload)(uint8_t ch, mwBuffHandle_t mwBuff);
	uint8_t (*HTTP_POST)(const char *url, uint16_t url_len, const uint8_t *payload, uint16_t payload_len, uint8_t *response);
	uint8_t EnableLoc;
	

}mwM26Handle_t;

extern mwM26Handle_t mwM26;


__weak void *mwM26MallocHook(uint16_t size);
__weak void mwM26FreeHook(void *p);
__weak void mwM26DelayMSHook(uint32_t timespan);
__weak void mwM26IdleHook(mwBuffHandle_t pack);
__weak void mwM26RecvPayloadHook(uint8_t ch, mwBuffHandle_t pack);
__weak void mwM26SendHook(mwBuffHandle_t pack);
__weak void mwM26SendPayloadDoneHook(void);
__weak void mwM26MallocErrorHook(void);
__weak void mwM26ParseErrorHook(void);
__weak void mwM26TCPConnOKHook(uint8_t ch);
__weak void mwM26TCPConnERRHook(uint8_t ch);
__weak void mwM26GPRSConnOKHook(void);
__weak void mwM26GPRSConnERRHook(void);
__weak void mwM26StartResetHook(void);

void mwM26Init(uint8_t *rxbuff, uint16_t *rxbufflen, uint64_t *sticks);

#endif
