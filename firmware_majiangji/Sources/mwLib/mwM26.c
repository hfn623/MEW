#include "mwM26.h"

#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "mwbuff.h"

uint8_t reading_cmd;

mwM26Handle_t mwM26;

static uint64_t *systicks;

static uint8_t socketEnableBits;
static uint8_t socketStateBits;

static uint64_t lastCommTime[CHANNEL_COUNT];
static uint64_t lastConnTime[CHANNEL_COUNT];
static uint64_t lastReadTime[CHANNEL_COUNT];

static uint64_t lastConnOpTime;

static mwBuffHandle_t sendStream;
static int8_t sendCH;
static mwBuffHandle_t recvStream;
static uint8_t socketConnectingChannel;
static mwBuffHandle_t heartbeatPack;

static uint8_t hblength;
static uint8_t hbcontent[16];


__weak void *mwM26MallocHook(uint16_t size)
{
	return malloc(size);
}

__weak void mwM26FreeHook(void *p)
{
	free(p);
}
__weak void mwM26DelayMSHook(uint32_t timespan)
{
}
__weak void mwM26IdleHook(void)
{
}

__weak void mwM26SocketRecvStartHook(uint8_t ch)
{
}
__weak void mwM26SocketRecvDoneHook(uint8_t ch, uint8_t *buff, uint16_t len)
{
}

__weak void mwM26SocketRecvErrHook(uint8_t ch)
{
}

__weak void mwM26SendHook( uint8_t *buff, uint16_t len)
{
}

__weak void mwM26SocketSendStartHook(uint8_t ch)
{
}

__weak void mwM26SocketSendDoneHook(uint8_t ch)
{
}

__weak void mwM26SocketSendErrHook(uint8_t ch)
{
}


__weak void mwM26MallocErrHook(void)
{
}

__weak void mwM26ParseErrHook(void)
{
}

__weak void mwM26SocketConnDoneHook(uint8_t ch)
{
}

__weak void mwM26SocketConnErrHook(uint8_t ch, int8_t reason)
{
}

__weak void mwM26SocketDisconnHook(uint8_t ch)
{
}


__weak void mwM26GPRSConnDoneHook(void)
{
}

__weak void mwM26GPRSConnErrHook(void)
{
}

__weak void mwM26ResetStartHook(void)
{
}

static uint8_t AT_X(char *req, char *resp, uint32_t timeout, uint32_t times)
{  
  int16_t fi;
	uint32_t time1;
	uint32_t time2 = 0;
	
	mwBuffHandle_t pReq;
	mwBuffHandle_t pResp;
	mwBuffHandle_t pRecv;
	
	pReq.pBuff = (uint8_t *)req;
	pReq.Length = strlen((char *)pReq.pBuff);
	
	pResp.pBuff = (uint8_t *)resp;
	pResp.Length = strlen((char *)pResp.pBuff);
	
	
  do
  {
		time1 = 0;
		
		*(recvStream.pLength) = 0;
    mwM26SendHook(pReq.pBuff, pReq.Length);
    

		pRecv.pBuff = recvStream.pBuff;		
		pRecv.Length = *(recvStream.pLength);
		
		while(!mwBuffEndWith(pRecv, pResp) && time1 < timeout)
		{
			mwM26DelayMSHook(1);
			time1 ++;
			pRecv.Length = *(recvStream.pLength);
		}		
    
    fi = mwBuffSearch(pRecv, pResp);
    if(fi >= 0)
    {
			*(recvStream.pLength) = 0;
      return 1;
    }
		
		mwM26DelayMSHook(timeout);
		time2 ++;
  }while(time2 < times);
	
	*(recvStream.pLength) = 0;
	return 0;
}

static uint8_t AT_GSN(void)
{  
	char *req = "AT+GSN\r";
	char *resp = "AT+GSN\r\r\n";
	char *ok = "\r\n\r\nOK\r\n";
		
	int16_t fi, fi2;
	uint32_t times;
	

	
	mwBuffHandle_t pReq;
	mwBuffHandle_t pResp;
	mwBuffHandle_t pRecv;
	mwBuffHandle_t pOK;
	

	
	//sprintf(req, "AT+QILOCIP\r", ch, readMax);

	pReq.pBuff = (uint8_t *)req;
	pReq.Length = strlen((char *)pReq.pBuff);
	
	pResp.pBuff = (uint8_t *)resp;
	pResp.Length = strlen((char *)pResp.pBuff);
	
	pOK.pBuff = (uint8_t *)ok;
	pOK.Length = 8;
		
  //while(1)
  {
		times = 0;
		
		*(recvStream.pLength) = 0;
    mwM26SendHook(pReq.pBuff, pReq.Length);
		
		
		pRecv.pBuff = recvStream.pBuff;		
		pRecv.Length = *(recvStream.pLength);
		
		while(!mwBuffEndWith(pRecv, pOK) && times < AT_CMD_TIMEOUT)
		{
			mwM26DelayMSHook(1);
			times ++;
			pRecv.Length = *(recvStream.pLength);
		}
		
		
		//sprintf(resp, "AT+QIRD=0,1,%d,%d\r\r\nOK\r\n", ch, readMax);
		pResp.pBuff = (uint8_t *)resp;
		pResp.Length = strlen((char *)pResp.pBuff);
		fi = mwBuffSearch(pRecv, pResp);
		if(fi >= 0)
		{			
			fi2 = mwBuffSearch(pRecv, pOK);
			if(fi2 > 0)
			{
				memset(mwM26.IMEI, 0, 20);
				memcpy(mwM26.IMEI, pRecv.pBuff + pResp.Length, fi2 - pResp.Length);
				*(recvStream.pLength) = 0;
				return 1;
			}
		}
		//break;
	}
	//memset(RXHandle.hBuff, 0, *(RXHandle.hLength));
	*(recvStream.pLength) = 0;	
	return 0;
}
static uint8_t AT_QIDNSGIP(char *pIP, char *pOut , uint32_t sec)
{  
	char req[128];
	char resp[128];
	char *_r_n = "\r\n";
//	char *err = "ERROR";
		
	int16_t fi;
	uint32_t times;
	

	
	mwBuffHandle_t pReq;
	mwBuffHandle_t pResp;
	mwBuffHandle_t pRecv;
	mwBuffHandle_t p_r_n;
//	mwBuffHandle_t pErr;

	

	sprintf(req, "AT+QIDNSGIP=\"%s\"\r", pIP);
	sprintf(resp, "AT+QIDNSGIP=\"%s\"\r\r\nOK\r\n\r\n", pIP);

	pReq.pBuff = (uint8_t *)req;
	pReq.Length = strlen((char *)pReq.pBuff);
	
	pResp.pBuff = (uint8_t *)resp;
	pResp.Length = strlen((char *)pResp.pBuff);
	
	p_r_n.pBuff = (uint8_t *)_r_n;
	p_r_n.Length = strlen((char *)p_r_n.pBuff);
	
//	pErr.pBuff = (uint8_t *)err;
//	pErr.Length = strlen((char *)pErr.pBuff);
	
		
  //while(1)
  {
		times = 0;
		
		*(recvStream.pLength) = 0;
    mwM26SendHook(pReq.pBuff, pReq.Length);
		
		
		pRecv.pBuff = recvStream.pBuff;		
		pRecv.Length = *(recvStream.pLength);
		
		fi = -1;
		while(fi < 0 && times < sec * 1000)
		{
			mwM26DelayMSHook(1);
			times ++;
			pRecv.Length = *(recvStream.pLength);
			fi = mwBuffSearch(pRecv, pResp);
		}		
		
		times = 0;
		*(recvStream.pLength) = 0;
		
		pRecv.pBuff = recvStream.pBuff;		
		pRecv.Length = *(recvStream.pLength);
		
		fi = -1;
		while(fi < 0 && times < sec * 1000)
		{
			mwM26DelayMSHook(1);
			times ++;
			pRecv.Length = *(recvStream.pLength);
			fi = mwBuffSearch(pRecv, p_r_n);
		}
		
		if(fi >= 0)
		{
			memcpy(pOut, pRecv.pBuff, fi);
			pOut[fi] = 0;
		}
		
		//break;
	}
	//mwM26DelayMSHook(5000);
	//memset(RXHandle.hBuff, 0, *(RXHandle.hLength));
	*(recvStream.pLength) = 0;	
	return 1;
}

static uint8_t AT_QCELLLOC(void)
{  
	char *req = "AT+QCELLLOC=1\r";
	char *resp = "\r\n+QCELLLOC: ";
	char *ok = "\r\n\r\nOK\r\n";
		
	char databuff[32];
	
	int16_t fi, fi2;
	uint32_t times;
	

	
	mwBuffHandle_t pReq;
	mwBuffHandle_t pResp;
	mwBuffHandle_t pRecv;
	mwBuffHandle_t pOK;
	

	
	//sprintf(req, "AT+QILOCIP\r", ch, readMax);

	pReq.pBuff = (uint8_t *)req;
	pReq.Length = strlen((char *)pReq.pBuff);
	
	pResp.pBuff = (uint8_t *)resp;
	pResp.Length = strlen((char *)pResp.pBuff);
	
	pOK.pBuff = (uint8_t *)ok;
	pOK.Length = 8;
	

	
  //while(1)
  {
		times = 0;
		
		*(recvStream.pLength) = 0;
    mwM26SendHook(pReq.pBuff, pReq.Length);
		
		
		pRecv.pBuff = recvStream.pBuff;		
		pRecv.Length = *(recvStream.pLength);
		
		while(!mwBuffEndWith(pRecv, pOK) && times < 30000)
		{
			mwM26DelayMSHook(1);
			times ++;
			pRecv.Length = *(recvStream.pLength);
		}

		pResp.pBuff = (uint8_t *)resp;
		pResp.Length = strlen((char *)pResp.pBuff);
		fi = mwBuffSearch(pRecv, pResp);
		if(fi >= 0)
		{			
			fi2 = mwBuffSearch(pRecv, pOK);
			if(fi2 > 0)
			{
				memset(databuff, 0, 32);
				memcpy(databuff, pRecv.pBuff + pResp.Length, fi2 - pResp.Length);
				for(fi = 0; fi < fi2 - pResp.Length; fi ++)
				{
					if(databuff[fi] == ',')
					{
						memcpy(mwM26.Longitude, databuff, fi);
						memcpy(mwM26.Latitude, databuff + fi + 1, fi2 - pResp.Length - fi);
					}
				}
				*(recvStream.pLength) = 0;
				return 1;
			}
		}
		
		
//		mwM26DelayMSHook(1000);
//		times2 ++;
//		if(times2 >= 30)
//		{
//			break;
//		}
	}

	*(recvStream.pLength) = 0;	
	return 0;
}
static uint8_t AT_CLOSE(uint8_t ch, uint32_t timeout)
{
	int16_t fi;
	uint32_t times;
	
	char req[32];
	char resp[32];

	mwBuffHandle_t pRecv;	
	mwBuffHandle_t pReq;
	mwBuffHandle_t pResp;
	
	sprintf(req, "AT+QICLOSE=%d\r", ch);
	pReq.pBuff = (uint8_t *)req;
	pReq.Length = strlen((char *)pReq.pBuff);
	
	sprintf(resp, "%d, CLOSE OK\r\n", ch);
	pResp.pBuff = (uint8_t *)resp;
	pResp.Length = strlen((char *)pResp.pBuff);
	
	times = 0;
	
	*(recvStream.pLength) = 0;
	
	pRecv.pBuff = recvStream.pBuff;
	pRecv.Length = *(recvStream.pLength);
	
	mwM26SendHook(pReq.pBuff, pReq.Length);
	
	fi = -1;		
	while(fi < 0 && times < timeout)
	{
		mwM26DelayMSHook(1);
		times ++;
		pRecv.Length = *(recvStream.pLength);
		fi = mwBuffSearch(pRecv, pResp);
		if(fi >= 0)
		{
			return 1;
		}
	}
	
	*(recvStream.pLength) = 0;	
	return 0;
}
static int8_t AT_QIOPEN(uint8_t ch, char *pIP, uint16_t pPORT, uint32_t sec)
{
	char req[64];
	char resp[32];
	char resp2[32];
	
	int16_t fi;
	uint32_t times;
	
	mwBuffHandle_t pReq;
	mwBuffHandle_t pResp;
	mwBuffHandle_t pRecv;
	
	mwBuffHandle_t pResp2;
	
//	mwBuffHandle_t pCOK;
//	BW_PackHandle_t pFAIL;
	
	sprintf(req, "AT+QIOPEN=%d,\"TCP\",\"%s\",%d\r", ch, pIP, pPORT);
	sprintf(resp, "%d, CONNECT OK\r\n", ch);
	sprintf(resp2, "%d, CONNECT FAIL\r\n", ch);
	
	pReq.pBuff = (uint8_t *)req;
	pReq.Length = strlen((char *)pReq.pBuff);
	
	pResp.pBuff = (uint8_t *)resp;
	pResp.Length = strlen((char *)pResp.pBuff);
	
	pResp2.pBuff = (uint8_t *)resp2;
	pResp2.Length = strlen((char *)pResp2.pBuff);	

	
  {
		times = 0;
		
		*(recvStream.pLength) = 0;
    mwM26SendHook(pReq.pBuff, pReq.Length);
    
		mwM26DelayMSHook(AT_CMD_TIMEOUT);
		
		pRecv.pBuff = recvStream.pBuff;
		pRecv.Length = *(recvStream.pLength);
		fi = -1;

		while(times < sec * 1000)
		{
			mwM26DelayMSHook(1);
			times ++;
			pRecv.Length = *(recvStream.pLength);
			fi = mwBuffSearch(pRecv, pResp);
			if(fi >= 0)
			{
				*(recvStream.pLength) = 0;
				return 1;
			}
			fi = mwBuffSearch(pRecv, pResp2);
			if(fi >= 0)
			{
				*(recvStream.pLength) = 0;
				return -2;
			}
		}
  }
	
	*(recvStream.pLength) = 0;
	return -1;
}
static uint8_t AT_QISEND(uint8_t ch, uint8_t *buff, uint16_t len)
{
	char req[32];
	
	sprintf((char *)req, "AT+QISEND=%d,%d\r", ch, len);
	
	if(AT_X(req, "\r\r\n> ", 1000, 0))
	{
		if(AT_X((char *)buff, "SEND OK", 1000, 0))
		{
			return 1;
		}
	}	
	return 0;
}

static uint8_t AT_QIRD(uint8_t ch, char *pIP, uint16_t pPORT, uint16_t readMax, uint32_t sec)
{  
	char req[20];
	char resp[64];
		
	int16_t fi;
	uint32_t times;
	
	char dlenBuff[6] = {0};
	
	uint8_t i;
	
	mwBuffHandle_t pReq;
	mwBuffHandle_t pResp;
	mwBuffHandle_t pRecv;
	mwBuffHandle_t pOK;
	mwBuffHandle_t pERROR;
	
	mwBuffHandle_t payload;
	
	sprintf(req, "AT+QIRD=0,1,%d,%d\r", ch, readMax);

	pReq.pBuff = (uint8_t *)req;
	pReq.Length = strlen((char *)pReq.pBuff);

	sprintf(resp, "AT+QIRD=0,1,%d,%d\r\r\nOK\r\n", ch, readMax);	
	pResp.pBuff = (uint8_t *)resp;
	pResp.Length = strlen((char *)pResp.pBuff);
	
	pOK.pBuff = (uint8_t *)"\r\nOK\r\n";
	pOK.Length = 6;
	
	pERROR.pBuff = (uint8_t *)"\r\nERROR\r\n";
	pERROR.Length = 9;
		
  while(1)
  {
		times = 0;
		
		*(recvStream.pLength) = 0;
    mwM26SendHook(pReq.pBuff, pReq.Length);
		
		while(*(recvStream.pLength) == 0 && times < AT_CMD_TIMEOUT)
		{
			mwM26DelayMSHook(1);
			times ++;
		}
		//while(UART2IsSending);
		
//		while(*(RXHandle.hLength) == 0 && times < DELAY_TIME)
//		{
//			delay_ms(1);
//			times ++;
//		}
		
//		do
//		{
//		while(1)
//		{
		
		
		
		
		pRecv.pBuff = recvStream.pBuff;
		pRecv.pLength = (recvStream.pLength);
		
		do
		{
			pRecv.Length = *pRecv.pLength;
		}
		while((!mwBuffEndWith(pRecv, pOK)) && (!mwBuffEndWith(pRecv, pERROR)));
		
		sprintf(resp, "AT+QIRD=0,1,%d,%d\r\r\nOK\r\n", ch, readMax);
		pResp.pBuff = (uint8_t *)resp;
		pResp.Length = strlen((char *)pResp.pBuff);
		fi = mwBuffSearch(pRecv, pResp);
		if(fi >= 0)
		{			
			*(recvStream.pLength) = 0;	
			return 1;
		}
		else
		{
			sprintf(resp, "AT+QIRD=0,1,%d,%d\r\r\nERROR\r\n", ch, readMax);
			pResp.pBuff = (uint8_t *)resp;
			pResp.Length = strlen((char *)pResp.pBuff);
			fi = mwBuffSearch(pRecv, pResp);
			if(fi >= 0)
			{
				*(recvStream.pLength) = 0;	
				return 0;
			}
			else
			{
				//sprintf(resp, "AT+QIRD=0,1,%d,%d\r\r\n+QIRD: %s:%d,TCP,", ch, readMax, pIP, pPORT);
				sprintf(resp, "+QIRD: %s:%d,TCP,", pIP, pPORT);
				pResp.pBuff = (uint8_t *)resp;
				pResp.Length = strlen((char *)pResp.pBuff);
				fi = mwBuffSearch(pRecv, pResp);	
				if(fi >= 0)
				{
					mwM26SocketRecvStartHook(ch);
					for(i = 0; i < 4; i ++)
					{
						if(pRecv.pBuff[fi + pResp.Length + i] == '\r')
						{
							break;
						}
						dlenBuff[i] = pRecv.pBuff[fi + pResp.Length + i];
					}
					payload.Length = atoi(dlenBuff);	
					times = 0;
					while((*(recvStream.pLength) < fi + pResp.Length + i + 2 + payload.Length + 6) && (times < AT_CMD_TIMEOUT))
					{
						mwM26DelayMSHook(1);
						times ++;
					}
					if(mwBuffOffsetWith(fi + pResp.Length + i + 2 + payload.Length, pRecv, pOK))
					{
						payload.pBuff = mwM26MallocHook(payload.Length);
						if(payload.pBuff)
						{
							memcpy(payload.pBuff, fi + pRecv.pBuff + pResp.Length + i + 2, payload.Length);
							mwM26SocketRecvDoneHook(ch, payload.pBuff, payload.Length);
							mwM26FreeHook(payload.pBuff);
							*(recvStream.pLength) = 0;
							return 2;
						}
						else
						{
							mwM26MallocErrHook();
						}
					}
					else
					{
						mwM26ParseErrHook();
					}
				}	
				else
				{
					fi = -1;
				}	
			}			
		}
		break;
	}
	//memset(RXHandle.hBuff, 0, *(RXHandle.hLength));
	*(recvStream.pLength) = 0;	
	return 3;
}

static uint8_t AT_QHTTPREAD(uint16_t timeout, uint8_t *out)
{
	char req[20];
	char resp[40];
	char lenbuf[5] = {0};
		
	int16_t fi, fi2;
	uint32_t times;
	uint16_t payload_len;
	
//	uint8_t i;
	
	mwBuffHandle_t pReq;
	mwBuffHandle_t pResp;
	mwBuffHandle_t pRecv;
	mwBuffHandle_t pOK;
	mwBuffHandle_t pERROR;
	mwBuffHandle_t pLine;
	
//	mwBuffHandle_t payload;
	
	sprintf(req, "AT+QHTTPREAD=%d\r", 10);
	sprintf(resp, "AT+QHTTPREAD=%d\r\r\nCONNECT\r\n", 10);
		

	pReq.pBuff = (uint8_t *)req;
	pReq.Length = strlen((char *)pReq.pBuff);
	
	pResp.pBuff = (uint8_t *)resp;
	pResp.Length = strlen((char *)pResp.pBuff);
	
	pOK.pBuff = (uint8_t *)"\r\nOK\r\n";
	pOK.Length = 6;
	
	pERROR.pBuff = (uint8_t *)"\r\nERROR\r\n";
	pERROR.Length = 9;
	
	pLine.pBuff = (uint8_t *)"\r\n";
	pLine.Length = 2;
		
  while(1)
  {
		times = 0;
		
		*(recvStream.pLength) = 0;
    mwM26SendHook(pReq.pBuff, pReq.Length);
		
		while(*(recvStream.pLength) == 0 && times < timeout)
		{
			mwM26DelayMSHook(1);
			times ++;
		}
		
		pRecv.pBuff = recvStream.pBuff;
		pRecv.pLength = (recvStream.pLength);
		
		do
		{
			pRecv.Length = *pRecv.pLength;
		}
		while((!mwBuffEndWith(pRecv, pOK)) && (!mwBuffEndWith(pRecv, pERROR)) && times < timeout);
		
		
		pResp.pBuff = (uint8_t *)resp;
		pResp.Length = strlen((char *)pResp.pBuff);
		fi = mwBuffSearch(pRecv, pResp);
		if(fi >= 0)
		{
			fi2 = mwBuffSearchFrom(pRecv, fi + pResp.Length, pLine);
			if(fi2 >= 0)
			{
				memcpy(lenbuf, pRecv.pBuff + fi + pResp.Length, fi2 - fi - pResp.Length);
				payload_len = strtol(lenbuf, NULL, 16);
				memcpy(out, pRecv.pBuff + fi2 + 2, payload_len);
				*(recvStream.pLength) = 0;	
				return 1;
			}
			
		}
		
		break;
		
	}

	*(recvStream.pLength) = 0;	
	return 0;
}
static uint8_t socketStateGet(uint8_t ch)
{
	return socketStateBits & (1 << ch);
}

static void socketStateSet(uint8_t ch)
{
	socketStateBits |= (1 << ch);
}

static void socketStateClr(uint8_t ch)
{
	socketStateBits &= ~(1 << ch);
}

static uint8_t socketEnableGet(uint8_t ch)
{
	return socketEnableBits & (1 << ch);
}

static void socketEnable(uint8_t ch)
{
	socketEnableBits |= (1 << ch);
}

static void socketDisable(uint8_t ch)
{
	socketEnableBits &= ~(1 << ch);
}

static uint8_t Reset(void)
{
	if(!AT_X("AT\r", "AT\r\r\nOK\r\n", AT_CMD_TIMEOUT, 3))
	{
		return 0;
	}
	if(!AT_X("AT+QPOWD=1\r", "AT+QPOWD=1\r\r\n", AT_CMD_TIMEOUT, 3))
	{
		return 0;
	}
	//delay_ms(3000);
	return 1;
}

static uint8_t GPRSConn(void)
{
//	uint8_t i = 0;
	if(!AT_X("AT\r", "AT\r\r\nOK\r\n", AT_CMD_TIMEOUT, 30))
	{
		return 0;
	}
	if(!AT_GSN())
	{
		return 0;
	}
	if(!AT_X("AT+CGATT=0\r", "AT+CGATT=0\r\r\nOK\r\n", AT_CMD_TIMEOUT, 3))
	{
		return 0;
	}
	if(!AT_X("AT+CGATT=1\r", "\r\nOK\r\n", AT_CMD_TIMEOUT, 200))
	{
		return 0;
	}
	if(!AT_X("AT+QIDNSIP=1\r", "AT+QIDNSIP=1\r\r\nOK\r\n", AT_CMD_TIMEOUT, 3))
	{
		return 0;
	}	
	if(!AT_X("AT+QINDI=1\r", "AT+QINDI=1\r\r\nOK\r\n", AT_CMD_TIMEOUT, 3))
	{
		return 0;
	}	
	if(!AT_X("AT+QIMUX=1\r", "AT+QIMUX=1\r\r\nOK\r\n", AT_CMD_TIMEOUT, 3))
	{
		return 0;
	}
	if(!AT_X("AT+QISDE=0\r", "AT+QISDE=0\r\r\nOK\r\n", AT_CMD_TIMEOUT, 3))
	{
		return 0;
	}
//	for(i = 0; i < 6; i ++)
//	{
//		if(socketEnableGet(i))
//		{
//			if(!AT_QIDNSGIP(mwM26.ADDR[i], mwM26.ADDR[i], 14))
//			{
//				return 0;
//			}
//		}
//	}
	if(mwM26.EnableLoc)
	{
		if(!AT_QCELLLOC())
		{
			return 0;
		}
	}
	return 1;
}


static void socketPush(uint8_t ch, uint8_t *txbuff, uint16_t len)
{
	//if(sendStream.Length > 0)
	{
		memcpy(sendStream.pBuff, txbuff, len);
		
		sendStream.Length = len;	
		
		sendCH = ch;
	}
}

static uint8_t socketSend(uint8_t ch, uint8_t *buff, uint16_t len)
{
	int8_t ret = AT_QISEND(ch, buff, len);
	if(ret)
	{
		lastCommTime[ch] = *systicks;
	}	
	return ret;
}



static uint8_t socketRecv(uint8_t ch)
{	
	uint8_t ret = 0;
	reading_cmd = 1;
	ret = AT_QIRD(ch, mwM26.IP[ch], mwM26.PORT[ch], 256, 1000);
	reading_cmd = 0;
	if(ret > 0)
	{
		lastReadTime[ch] = *systicks;
	}
	if(ret > 1)
	{		
		lastCommTime[ch] = *systicks;
	}	
	return ret;
}

static int8_t socketConn(uint8_t ch)
{
	int8_t ret = AT_QIOPEN(ch, mwM26.IP[ch], mwM26.PORT[ch], 10);
	lastConnTime[ch] = *systicks;
	if(ret == 1)
	{
		lastCommTime[ch] = *systicks;
	}
	return ret;
}


static void schedule_HTTP_NoOS(void)
{
	if(mwM26.WorkState == M26_WS_RESET)
	{
		mwM26.WorkState = M26_WS_GPRS_CONNECTING;
	}	
	if(mwM26.WorkState == M26_WS_GPRS_CONNECTED)
	{		
		mwM26.WorkState = M26_WS_IDLE;
	}
	if(mwM26.WorkState == M26_WS_TCP_CONNECTED)
	{
		mwM26.WorkState = M26_WS_IDLE;
	}
	if(mwM26.WorkState == M26_WS_IDLE)
	{
	}
	
	if(mwM26.WorkState == M26_WS_RESETING)
	{		
		mwM26ResetStartHook();
		if(Reset())
		{
			mwM26.WorkState = M26_WS_RESET;
		}
	}		
	if(mwM26.WorkState == M26_WS_GPRS_CONNECTING)
	{
		if(GPRSConn())
		{
			mwM26.WorkState = M26_WS_GPRS_CONNECTED;
			mwM26GPRSConnDoneHook();
		}
		else
		{
			mwM26GPRSConnErrHook();
			mwM26.WorkState = M26_WS_RESETING;
		}		
	}
}
static void schedule_Socket_NoOS(void)
{
	uint8_t i;
	int8_t res;
//	int8_t incommingCH;
//	if(*RXHandle.hLength > 0)
//	{
//		UART1Send(RXHandle.hBuff, *RXHandle.hLength);
//	}
	
	if(mwM26.WorkState == M26_WS_RESETING)
	{		
		mwM26ResetStartHook();
		if(Reset())
		{
			mwM26.WorkState = M26_WS_RESET;
		}
	}		
	if(mwM26.WorkState == M26_WS_GPRS_CONNECTING)
	{
		if(GPRSConn())
		{			
			mwM26GPRSConnDoneHook();
			mwM26.WorkState = M26_WS_GPRS_CONNECTED;
		}
		else
		{
			mwM26GPRSConnErrHook();
			mwM26.WorkState = M26_WS_RESETING;
		}
		
	}
	if(mwM26.WorkState == M26_WS_TCP_CONNECTING)
	{		
		if(!AT_QIDNSGIP(mwM26.ADDR[socketConnectingChannel], mwM26.IP[socketConnectingChannel], 30))
		{
			//return 0;
		}
		if(AT_CLOSE(socketConnectingChannel, AT_CMD_TIMEOUT))
		{
			socketStateClr(socketConnectingChannel);
		}
		else
		{
			mwM26SocketConnErrHook(socketConnectingChannel, -3);
			//close fail
		}	
		res = socketConn(socketConnectingChannel);
		lastConnOpTime = *systicks;
		if( res > 0)
		{
			socketStateSet(socketConnectingChannel);
			mwM26SocketConnDoneHook(socketConnectingChannel);
			//gprs conn ok
		}
		else
		{
			mwM26SocketConnErrHook(socketConnectingChannel, res);
//			if(res == -1)
//			{
//				
//				//find connect fail
//			}
//			else if(res == -2)
//			{
//				//time out
//			}
					
		}
		mwM26.WorkState = M26_WS_IDLE;
		//LED1_Toggle(0);
	}
	
	if(mwM26.WorkState == M26_WS_RESET)
	{
		mwM26.WorkState = M26_WS_GPRS_CONNECTING;
	}	
	if(mwM26.WorkState == M26_WS_GPRS_CONNECTED)
	{		
		mwM26.WorkState = M26_WS_IDLE;
	}
	if(mwM26.WorkState == M26_WS_TCP_CONNECTED)
	{
		mwM26.WorkState = M26_WS_IDLE;
	}
	if(mwM26.WorkState == M26_WS_IDLE)
	{
		for(i = 0; i < CHANNEL_COUNT; i ++)
		{
			if(socketEnableGet(i))//通道激活
			{
				if(socketStateGet(i))//通道已连接
				{
					if(sendStream.Length > 0 && sendCH == i)//如果有数据待发送
					{
						mwM26SocketSendStartHook(i);
						if(!socketSend(i, sendStream.pBuff, sendStream.Length))
						{
							socketStateClr(i);
							mwM26SocketSendErrHook(i);
						}
						else
						{
							mwM26SocketSendDoneHook(i);
							sendStream.Length = 0;
						}
					}
					else//没有数据需要发送
					{
						//长时间没有数据交互，发送心跳包
						if(*systicks - lastCommTime[i] >= 1000 * mwM26.HeartbeatInterval_Sec)
						{
							socketPush(i, heartbeatPack.pBuff, heartbeatPack.Length); 
						}
						else//空闲
						{						
							//读数据
							if(*systicks - lastReadTime[i] >= mwM26.ReadInterval_MilliSec)
							{
								if(!socketRecv(i))
								{
									socketStateClr(i);
									mwM26SocketRecvErrHook(i);
								}
							}
						}
					}					
					//Locking = 0;
				}
				else//通道未连接
				{
					if(*systicks - lastConnOpTime >= 1000 * mwM26.ConnectionInterval_Sec)
					{
						if(*systicks - lastConnTime[i] >= 1000 * mwM26.ConnectionInterval_Sec)
						{
							//socketStateClr(i);
							socketConnectingChannel = i;
							mwM26.WorkState = M26_WS_TCP_CONNECTING;
							
							break;
						}
					}
				}
			}
		}
	}
}

static uint8_t httpPOST(const char *url, uint16_t url_len, const uint8_t *payload, uint16_t payload_len, uint8_t *response)
{
	char req[32];
	char resp[64];
	
	sprintf(req, "AT+QHTTPURL=%d,10\r", url_len);
	sprintf(resp, "AT+QHTTPURL=%d,10\r\r\nCONNECT\r\n", url_len);
	if(!AT_X(req, resp, 10000, 1))
	{
		return 0;
	}
	if(!AT_X((char *)url, "\r\nOK\r\n", AT_CMD_TIMEOUT, 1))
	{
		return 0;
	}
	sprintf(req, "AT+QHTTPPOST=%d,10,10\r", payload_len);
	sprintf(resp, "AT+QHTTPPOST=%d,10,10\r\r\nCONNECT\r\n", payload_len);
	if(!AT_X(req, resp, 10000, 3))
	{
		return 0;
	}
	if(!AT_X((char *)payload, "\r\nOK\r\n", 30000, 3))
	{
		return 0;
	}
	
	if(!AT_QHTTPREAD(10, response))
	{
		return 0;
	}
	
	return 1;
}



void mwM26Init(uint8_t *txbuff, uint16_t *txbufflen, uint8_t *rxbuff, uint16_t *rxbufflen, uint64_t *sticks)
{	
	systicks = sticks;
	
	sendStream.pBuff = txbuff;
	sendStream.pLength = txbufflen;
	
	recvStream.pBuff = rxbuff;
	recvStream.pLength = rxbufflen;
	
	socketStateBits = 0;
	socketEnableBits = 0;
	
	mwM26.WorkState = M26_WS_RESETING;		
	mwM26.Schedule_Socket_NoOS = schedule_Socket_NoOS;
	mwM26.Schedule_HTTP_NoOS = schedule_HTTP_NoOS;
	mwM26.SocketSend = socketPush;	

	mwM26.HTTP_POST = httpPOST;
	
	mwM26.SocketEnable = socketEnable;
	mwM26.SocketEnableGet = socketEnableGet;
	mwM26.SocketDisable = socketDisable;
	
	hbcontent[0] = '*';
	hblength = 1;
	heartbeatPack.pBuff = hbcontent;
	heartbeatPack.Length = hblength;
	
	mwM26.HeartbeatInterval_Sec = 60;
	mwM26.ConnectionInterval_Sec = 10;
	mwM26.ReadInterval_MilliSec = 100;
	mwM26.EnableLoc = 0;
}


