#include "mwM26.h"

#include "string.h"
#include "stdio.h"
#include "stdlib.h"

mwM26Handle_t mwM26;

static uint64_t *systicks;

static uint8_t TCPEnableBits;
static uint8_t TCPStateBits;

static uint64_t lastCommTime[CHANNEL_COUNT];
static uint64_t lastConnTime[CHANNEL_COUNT];
static uint64_t lastReadTime[CHANNEL_COUNT];

static mwBuffHandle_t recvStream;
static uint8_t TCPConnectingChannel;
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
__weak void mwM26IdleHook(mwBuffHandle_t pack)
{
}

__weak void mwM26RecvPayloadHook(uint8_t ch, mwBuffHandle_t pack)
{
}

__weak void mwM26SendHook(mwBuffHandle_t pack)
{
}

__weak void mwM26SendPayloadDoneHook(void)
{
}

__weak void mwM26MallocErrorHook(void)
{
}

__weak void mwM26ParseErrorHook(void)
{
}

__weak void mwM26TCPConnOKHook(uint8_t ch)
{
}

__weak void mwM26TCPConnERRHook(uint8_t ch)
{
}

__weak void mwM26GPRSConnOKHook(void)
{
}

__weak void mwM26GPRSConnERRHook(void)
{
}

__weak void mwM26StartResetHook(void)
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
    mwM26SendHook(pReq);
    

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
		
		mwM26DelayMSHook(1000);
		time2 ++;
  }while(time2 <= times);
	
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
    mwM26SendHook(pReq);
		
		
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
    mwM26SendHook(pReq);
		
		
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
static uint8_t AT_QIOPEN(uint8_t ch, char *pIP, uint16_t pPORT, uint32_t sec)
{
	char req[64];
	char resp[32];
	
	int16_t fi;
	uint32_t times;
	
	mwBuffHandle_t pReq;
	mwBuffHandle_t pResp;
	mwBuffHandle_t pRecv;
	
	mwBuffHandle_t pOK;
//	BW_PackHandle_t pFAIL;
	
	sprintf(req, "AT+QIOPEN=%d,\"TCP\",\"%s\",%d\r", ch, pIP, pPORT);
	sprintf(resp, "%d, CONNECT", ch);
	
	pReq.pBuff = (uint8_t *)req;
	pReq.Length = strlen((char *)pReq.pBuff);
	
	pResp.pBuff = (uint8_t *)resp;
	pResp.Length = strlen((char *)pResp.pBuff);
	
	pOK.pBuff = (uint8_t *)"CONNECT OK\r\n";
	pOK.Length = 12;
	
//	pFAIL.hBuff = (uint8_t *)"CONNECT FAIL\r\n";
//	pFAIL.Length = 14;
	
	
  //while(1)
  {
		times = 0;
		
		*(recvStream.pLength) = 0;
    mwM26SendHook(pReq);
    
		mwM26DelayMSHook(AT_CMD_TIMEOUT);
		//while(UART2IsSending);
		
//		while(*(RXHandle.hLength) == 0 && times < DELAY_TIME)
//		{
//			delay_ms(1);
//			times ++;
//		}
		pRecv.pBuff = recvStream.pBuff;
		pRecv.Length = *(recvStream.pLength);
		fi = mwBuffSearch(pRecv, pResp);
		while(fi < 0 && times < sec * 1000)
		{
			mwM26DelayMSHook(1);
			times ++;
			pRecv.Length = *(recvStream.pLength);
			fi = mwBuffSearch(pRecv, pResp);
		}
		
		mwM26DelayMSHook(100);

		if(mwBuffSearch(pRecv, pOK) >= 0)
		{
			*(recvStream.pLength) = 0;
			return 1;
		}

		sprintf(req, "AT+QICLOSE=%d\r", ch);
    mwM26SendHook(pReq);
//    fi = BW_PackSearch(pRecv, pResp);
//    if(fi >= 0)
//    {
//			*(RXHandle.hLength) = 0;
//      return 1;
//    }
		
//		delay_ms(10000);
//		times2 ++;
//		if(times2 >= sec)
//		{
//			break;
//		}
  }
	*(recvStream.pLength) = 0;
	return 0;
}
static uint8_t AT_QISEND(uint8_t ch, mwBuffHandle_t pack)
{
	char req[32];
	
//	int16_t fi;
//	uint32_t times;
	
//	BW_PackHandle_t pReq;
//	BW_PackHandle_t pResp;
//	
//	BW_PackHandle_t pOK;
//	BW_PackHandle_t pFAIL;
	
	sprintf((char *)req, "AT+QISEND=%d,%d\r", ch, pack.Length);
	
	if(AT_X(req, "\r\r\n> ", AT_CMD_TIMEOUT, 0))
	{
		if(AT_X((char *)pack.pBuff, "SEND OK", AT_CMD_TIMEOUT, 3))
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
	
	pOK.pBuff = "\r\nOK\r\n";
	pOK.Length = 6;
	
	pERROR.pBuff = "\r\nERROR\r\n";
	pERROR.Length = 9;
		
  while(1)
  {
		times = 0;
		
		*(recvStream.pLength) = 0;
    mwM26SendHook(pReq);
		
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
							mwM26RecvPayloadHook(ch, payload);
							mwM26FreeHook(payload.pBuff);
							*(recvStream.pLength) = 0;
							return 2;
						}
						else
						{
							mwM26MallocErrorHook();
						}
					}
					else
					{
						mwM26ParseErrorHook();
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
	
	pOK.pBuff = "\r\nOK\r\n";
	pOK.Length = 6;
	
	pERROR.pBuff = "\r\nERROR\r\n";
	pERROR.Length = 9;
	
	pLine.pBuff = "\r\n";
	pLine.Length = 2;
		
  while(1)
  {
		times = 0;
		
		*(recvStream.pLength) = 0;
    mwM26SendHook(pReq);
		
//		while(*(recvStream.pLength) == 0 && times < timeout)
//		{
//			mwM26DelayMSHook(1);
//			times ++;
//		}
		
		pRecv.pBuff = recvStream.pBuff;
		pRecv.pLength = (recvStream.pLength);
		
		do
		{
			mwM26DelayMSHook(1);
			times ++;
			pRecv.Length = *pRecv.pLength;			
			if(mwBuffEndWith(pRecv, pOK))
			{
				break;
			}
			if(mwBuffEndWith(pRecv, pERROR))
			{
				break;
			}
		}
		while(times < timeout);
		
		
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
static uint8_t TCPStateGet(uint8_t ch)
{
	return TCPStateBits & (1 << ch);
}

static void TCPStateSet(uint8_t ch)
{
	TCPStateBits |= (1 << ch);
}

static void TCPStateClr(uint8_t ch)
{
	TCPStateBits &= ~(1 << ch);
}

static uint8_t TCPEnableGet(uint8_t ch)
{
	return TCPEnableBits & (1 << ch);
}

static void TCPEnableSet(uint8_t ch)
{
	TCPEnableBits |= (1 << ch);
}

static void TCPEnableClr(uint8_t ch)
{
	TCPEnableBits &= ~(1 << ch);
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
	if(!AT_X("AT+CGATT=1\r", "AT+CGATT=1\r\r\nOK\r\n", AT_CMD_TIMEOUT, 30))
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
	if(mwM26.EnableLoc)
	{
		if(!AT_QCELLLOC())
		{
			return 0;
		}
	}
	return 1;
}
static uint8_t Send(uint8_t ch, mwBuffHandle_t pack)
{
	int8_t ret = AT_QISEND(ch, pack);
	if(ret)
	{
		lastCommTime[ch] = *systicks;
	}
	else
	{
		TCPStateClr(ch);
		mwM26TCPConnERRHook(ch);
	}
	return ret;
}



static uint8_t Recv(uint8_t ch)
{
	uint8_t ret = AT_QIRD(ch, mwM26.ADDR[ch], mwM26.PORT[ch], 1500, 1000);
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

static uint8_t TCPConn(uint8_t ch)
{
	uint8_t ret = AT_QIOPEN(ch, mwM26.ADDR[ch], mwM26.PORT[ch], 60);
	lastConnTime[ch] = *systicks;
	if(ret)
	{
		lastCommTime[ch] = *systicks;
	}
	return ret;
}


static void Schedule_HTTP_NoOS(void)
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
		mwM26StartResetHook();
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
			mwM26GPRSConnOKHook();
		}
		else
		{
			mwM26GPRSConnERRHook();
			mwM26.WorkState = M26_WS_RESETING;
		}		
	}
}
static void Schedule_TCP_NoOS(void)
{
	uint8_t i;
//	int8_t incommingCH;
//	if(*RXHandle.hLength > 0)
//	{
//		UART1Send(RXHandle.hBuff, *RXHandle.hLength);
//	}
	
	if(mwM26.WorkState == M26_WS_RESETING)
	{		
		mwM26StartResetHook();
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
			mwM26GPRSConnOKHook();
		}
		else
		{
			mwM26GPRSConnERRHook();
			mwM26.WorkState = M26_WS_RESETING;
		}
		
	}
	if(mwM26.WorkState == M26_WS_TCP_CONNECTING)
	{		
		if(TCPConn(TCPConnectingChannel))
		{
			TCPStateSet(TCPConnectingChannel);
			mwM26TCPConnOKHook(TCPConnectingChannel);
			//ok
		}
		else
		{
			TCPStateClr(TCPConnectingChannel);
			mwM26TCPConnERRHook(TCPConnectingChannel);
			//error
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
			if(TCPEnableGet(i))
			{
				if(TCPStateGet(i))
				{
					//Locking = 1;
					if(*systicks - lastReadTime[i] >= mwM26.ReadInterval_MilliSec)
					{
						if(*systicks - lastReadTime[i] >= mwM26.ReadInterval_MilliSec)
						{
							if(!Recv(i))
							{
								TCPStateClr(i);						
							}					
						}
						if(*systicks - lastCommTime[i] >= 1000 * mwM26.HeartbeatInterval_Sec)
						{
							if(!Send(i, heartbeatPack))
							{
								TCPStateClr(i);
							}
						}			
					}
					//Locking = 0;
				}
				else
				{
					if(*systicks - lastConnTime[i] >= 1000 * mwM26.ConnectionInterval_Sec)
					{
						TCPStateClr(i);
						TCPConnectingChannel = i;
						mwM26.WorkState = M26_WS_TCP_CONNECTING;
						break;
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
	
	if(!AT_QHTTPREAD(1000, response))
	{
		return 0;
	}
	
	return 1;
}
void mwM26Init(uint8_t *rxbuff, uint16_t *rxbufflen, uint64_t *sticks)
{	
	systicks = sticks;
	recvStream.pBuff = rxbuff;
	recvStream.pLength = rxbufflen;
	
	TCPStateBits = 0;
	TCPEnableBits = 0;
	
	
	mwM26.WorkState = M26_WS_RESETING;		
	mwM26.Schedule_TCP_NoOS = Schedule_TCP_NoOS;
	mwM26.Schedule_HTTP_NoOS = Schedule_HTTP_NoOS;
	mwM26.SendPayload = Send;	
	mwM26.TCPEnableSet = TCPEnableSet;
	mwM26.TCPEnableClr = TCPEnableClr;
	mwM26.TCPEnableGet = TCPEnableGet;
	mwM26.TCPStateGet = TCPStateGet;
	mwM26.HTTP_POST = httpPOST;
	
	hbcontent[0] = '*';
	hblength = 1;
	heartbeatPack.pBuff = hbcontent;
	heartbeatPack.Length = hblength;
	
	mwM26.HeartbeatInterval_Sec = 60;
	mwM26.ConnectionInterval_Sec = 10;
	mwM26.ReadInterval_MilliSec = 100;
	mwM26.EnableLoc = 0;
}


