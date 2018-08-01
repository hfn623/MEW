#include "mew_m26.h"
#include "mew_buff.h"

#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#define AT_CMD_TIMEOUT_MSEC		300
#define DNSGIP_TIMEOUT_SEC		60
#define CHANNEL_COUNT					6

static uint8_t isRecv;

mew_m26_Handle_t mew_m26;

static uint64_t *systicks;

static uint8_t socketEnableBits;
static uint8_t socketStateBits;

static uint64_t lastCommTime[CHANNEL_COUNT];
static uint64_t lastConnTime[CHANNEL_COUNT];
static uint64_t lastReadTime[CHANNEL_COUNT];

static uint64_t lastConnOpTime;
static mew_buff_Handle_t sendStream;
static mew_buff_Handle_t recvStream;
static mew_buff_Handle_t heartbeatPack;

static int8_t sendingChannel;
static uint8_t socketConnectingChannel;

static uint8_t hbLength;
static uint8_t hbContent[16];

static inline uint8_t socketStateGet(uint8_t ch);
static inline void socketStateSet(uint8_t ch);
static inline void socketStateClr(uint8_t ch);
static inline uint8_t socketEnableGet(uint8_t ch);
static inline void socketEnable(uint8_t ch);
static inline void socketDisable(uint8_t ch);

static uint8_t is_recv(void);

static void socketPush(uint8_t ch, uint8_t *txbuff, uint16_t len);

static void socket_Schedule_NoOS(void);


//必须重定义钩子
__weak void mew_m26_MallocErr_Hook(void){}
__weak void mew_m26_DelayMS_Hook(uint32_t timespan){}
__weak void mew_m26_SendBuff_Hook(uint8_t *buff, uint16_t len){}
//可选重定义钩子
__weak void *mew_m26_Malloc_Hook(uint16_t size)
{
	return malloc(size);
}
__weak void mew_m26_Free_Hook(void *p)
{
	free(p);
}
//事件钩子
__weak void mew_m26_ResetStart_Hook(void){}
__weak void mew_m26_Reset_Hook(void){}
__weak void mew_m26_IdleHook(void){}

__weak void mew_m26_GPRSConnDone_Hook(void){}
__weak void mew_m26_GPRSConnErr_Hook(void){}

__weak void mew_m26_SocketConnDone_Hook(uint8_t ch){}
__weak void mew_m26_SocketConnErr_Hook(uint8_t ch, int8_t reason){}
__weak void mew_m26_SocketDisconn_Hook(uint8_t ch, int8_t reason){}
__weak void mew_m26_SocketHeartbeat_Hook(uint8_t ch)
{
	socketPush(ch, heartbeatPack.pBuff, heartbeatPack.Length); 
}
__weak void mew_m26_SocketSendStart_Hook(uint8_t ch){}
__weak void mew_m26_SocketSendDone_Hook(uint8_t ch){}
__weak void mew_m26_SocketSendErr_Hook(uint8_t ch){}
__weak void mew_m26_SocketRecvStart_Hook(uint8_t ch){}
__weak void mew_m26_SocketRecvDone_Hook(uint8_t ch, uint8_t *buff, uint16_t len){}
__weak void mew_m26_SocketRecvErr_Hook(uint8_t ch){}
__weak void mew_m26_SocketRecvParseErr_Hook(uint8_t ch){}

void mew_m26_Init(uint8_t *txbuff, uint16_t *txbufflen, uint8_t *rxbuff, uint16_t *rxbufflen, uint64_t *sticks)
{	
	systicks = sticks;
	
	sendStream.pBuff = txbuff;
	sendStream.pLength = txbufflen;
	
	recvStream.pBuff = rxbuff;
	recvStream.pLength = rxbufflen;
	
	socketStateBits = 0;
	socketEnableBits = 0;
	
	mew_m26.WorkState = M26_WS_RESETING;		
	mew_m26.Socket_Schedule_NoOS = socket_Schedule_NoOS;
//	mew_m26.HTTP_Schedule_NoOS = HTTP_Schedule_NoOS;
	mew_m26.SocketSend = socketPush;	
	mew_m26.IsReceiving = is_recv;

//	mew_m26.HTTP_POST = httpPOST;
	
	mew_m26.SocketEnable = socketEnable;
	mew_m26.SocketEnableGet = socketEnableGet;
	mew_m26.SocketDisable = socketDisable;
	
	hbContent[0] = '*';
	hbLength = 1;
	
	heartbeatPack.pBuff = hbContent;
	heartbeatPack.Length = hbLength;
	
	mew_m26.HeartbeatInterval_Sec = 60;
	mew_m26.ConnectionInterval_Sec = 10;
	mew_m26.ReadInterval_MilliSec = 100;
	mew_m26.EnableLoc = 0;
}

static inline uint8_t socketStateGet(uint8_t ch)
{
	return socketStateBits & (1 << ch);
}

static inline void socketStateSet(uint8_t ch)
{
	socketStateBits |= (1 << ch);
}

static inline void socketStateClr(uint8_t ch)
{
	socketStateBits &= ~(1 << ch);
}

static inline uint8_t socketEnableGet(uint8_t ch)
{
	return socketEnableBits & (1 << ch);
}

static inline void socketEnable(uint8_t ch)
{
	socketEnableBits |= (1 << ch);
}

static inline void socketDisable(uint8_t ch)
{
	socketEnableBits &= ~(1 << ch);
}



static int8_t AT_X(char *req, char *resp, uint32_t timeout, uint32_t times)
{  
  int16_t fi;
	uint32_t time1;
	uint32_t time2 = 0;
	
	mew_buff_Handle_t pReq;
	mew_buff_Handle_t pResp;
	mew_buff_Handle_t pRecv;
	
	pReq.pBuff = (uint8_t *)req;
	pReq.Length = strlen((char *)pReq.pBuff);
	
	pResp.pBuff = (uint8_t *)resp;
	pResp.Length = strlen((char *)pResp.pBuff);
		
  do
  {
		time1 = 0;
		
		*(recvStream.pLength) = 0;
    mew_m26_SendBuff_Hook(pReq.pBuff, pReq.Length);    

		pRecv.pBuff = recvStream.pBuff;		
		pRecv.Length = *(recvStream.pLength);
		
		while(!mew_buff_EndWith(pRecv, pResp) && time1 < timeout)
		{
			mew_m26_DelayMS_Hook(1);
			time1 ++;
			pRecv.Length = *(recvStream.pLength);
		}		
    
    fi = mew_buff_Search(pRecv, pResp);
    if(fi >= 0)
    {
			*(recvStream.pLength) = 0;
      return 0;
    }
		
		mew_m26_DelayMS_Hook(timeout);
		time2 ++;
  }while(time2 < times);
	
	*(recvStream.pLength) = 0;
	return -1;
}

static int8_t AT_GSN(void)
{  
	char *req = "AT+GSN\r";
	char *resp = "AT+GSN\r\r\n";
	char *ok = "\r\n\r\nOK\r\n";
		
	int16_t fi, fi2;
	uint32_t times;
	
	mew_buff_Handle_t pReq;
	mew_buff_Handle_t pResp;
	mew_buff_Handle_t pRecv;
	mew_buff_Handle_t pOK;

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
    mew_m26_SendBuff_Hook(pReq.pBuff, pReq.Length);		
		
		pRecv.pBuff = recvStream.pBuff;		
		pRecv.Length = *(recvStream.pLength);
		
		while(!mew_buff_EndWith(pRecv, pOK) && times < AT_CMD_TIMEOUT_MSEC)
		{
			mew_m26_DelayMS_Hook(1);
			times ++;
			pRecv.Length = *(recvStream.pLength);
		}
		
		pResp.pBuff = (uint8_t *)resp;
		pResp.Length = strlen((char *)pResp.pBuff);
		fi = mew_buff_Search(pRecv, pResp);
		if(fi >= 0)
		{			
			fi2 = mew_buff_Search(pRecv, pOK);
			if(fi2 > 0)
			{
				memset(mew_m26.IMEI, 0, 20);
				memcpy(mew_m26.IMEI, pRecv.pBuff + pResp.Length, fi2 - pResp.Length);
				*(recvStream.pLength) = 0;
				return 0;
			}
		}
	}
	*(recvStream.pLength) = 0;	
	return -1;
}

static int8_t AT_QCELLLOC(void)
{  
	char *req = "AT+QCELLLOC=1\r";
	char *resp = "\r\n+QCELLLOC: ";
	char *ok = "\r\n\r\nOK\r\n";
		
	char databuff[32];
	
	int16_t fi, fi2;
	uint32_t times;
	
	mew_buff_Handle_t pReq;
	mew_buff_Handle_t pResp;
	mew_buff_Handle_t pRecv;
	mew_buff_Handle_t pOK;

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
    mew_m26_SendBuff_Hook(pReq.pBuff, pReq.Length);		
		
		pRecv.pBuff = recvStream.pBuff;		
		pRecv.Length = *(recvStream.pLength);
		
		while(!mew_buff_EndWith(pRecv, pOK) && times < 30000)
		{
			mew_m26_DelayMS_Hook(1);
			times ++;
			pRecv.Length = *(recvStream.pLength);
		}

		pResp.pBuff = (uint8_t *)resp;
		pResp.Length = strlen((char *)pResp.pBuff);
		fi = mew_buff_Search(pRecv, pResp);
		if(fi >= 0)
		{			
			fi2 = mew_buff_Search(pRecv, pOK);
			if(fi2 > 0)
			{
				memset(databuff, 0, 32);
				memcpy(databuff, pRecv.pBuff + pResp.Length, fi2 - pResp.Length);
				for(fi = 0; fi < fi2 - pResp.Length; fi ++)
				{
					if(databuff[fi] == ',')
					{
						memcpy(mew_m26.Longitude, databuff, fi);
						memcpy(mew_m26.Latitude, databuff + fi + 1, fi2 - pResp.Length - fi);
					}
				}
				*(recvStream.pLength) = 0;
				return 0;
			}
		}
	}

	*(recvStream.pLength) = 0;	
	return -1;
}

static int8_t AT_CLOSE(uint8_t ch, uint32_t timeout)
{
	int16_t fi;
	uint32_t times;
	
	char req[32];
	char resp[32];

	mew_buff_Handle_t pRecv;	
	mew_buff_Handle_t pReq;
	mew_buff_Handle_t pResp;
	
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
	
	mew_m26_SendBuff_Hook(pReq.pBuff, pReq.Length);
	
	fi = -1;		
	while(fi < 0 && times < timeout)
	{
		mew_m26_DelayMS_Hook(1);
		times ++;
		pRecv.Length = *(recvStream.pLength);
		fi = mew_buff_Search(pRecv, pResp);
		if(fi >= 0)
		{
			return 0;
		}
	}
	
	*(recvStream.pLength) = 0;	
	return -1;
}

static int8_t AT_QIDNSGIP(char *pIP, char *pOut , uint32_t sec)
{  
	char req[128];
	char resp[128];
	char *_r_n = "\r\n";
//	char *err = "ERROR";
		
	int16_t fi, fi_start, fi_stop, fi_add;
	uint32_t times;
	uint8_t pi, point_num = 0;
	
	mew_buff_Handle_t pReq;
	mew_buff_Handle_t pResp;
	mew_buff_Handle_t pRecv;
	mew_buff_Handle_t p_r_n;
	mew_buff_Handle_t point;
//	mwBuffHandle_t pErr;

	sprintf(req, "AT+QIDNSGIP=\"%s\"\r", pIP);
	sprintf(resp, "AT+QIDNSGIP=\"%s\"\r\r\nOK\r\n\r\n", pIP);

	pReq.pBuff = (uint8_t *)req;
	pReq.Length = strlen((char *)pReq.pBuff);
	
	pResp.pBuff = (uint8_t *)resp;
	pResp.Length = strlen((char *)pResp.pBuff);
	
	p_r_n.pBuff = (uint8_t *)_r_n;
	p_r_n.Length = strlen((char *)p_r_n.pBuff);
	
	point.pBuff = (uint8_t *)".";
	point.Length = 1;
	
//	pErr.pBuff = (uint8_t *)err;
//	pErr.Length = strlen((char *)pErr.pBuff);
			
  //while(1)
  {
		times = 0;
		
		*(recvStream.pLength) = 0;
    mew_m26_SendBuff_Hook(pReq.pBuff, pReq.Length);		
		
		pRecv.pBuff = recvStream.pBuff;		
		pRecv.Length = *(recvStream.pLength);
		
		fi = -1;
		while(fi < 0 && times < sec * 1000)
		{
			mew_m26_DelayMS_Hook(1);
			times ++;
			pRecv.Length = *(recvStream.pLength);
			fi = mew_buff_Search(pRecv, pResp);			
		}
		if(fi >= 0)
		{
			fi_start = fi + pResp.Length;
			
			fi = -1;
			while(fi < 0 && times < sec * 1000)
			{
				mew_m26_DelayMS_Hook(1);
				times ++;
				pRecv.Length = *(recvStream.pLength);
				fi = mew_buff_SearchFrom(pRecv, fi_start, p_r_n);
			}
			if(fi >= 0)
			{
				fi_stop = fi;
				
				fi_add = fi_start;			
				for(pi = 0; pi < 3; pi++)
				{
					fi = -1;
					while(fi < 0 && times < sec * 1000)
					{
						mew_m26_DelayMS_Hook(1);
						times ++;
						pRecv.Length = *(recvStream.pLength);
						fi = mew_buff_SearchFrom(pRecv, fi_add, point);
					}
					if(fi >= 0)
					{
						fi_add += point.Length;
						point_num++;
					}
				}
				if(point_num == 3)
				{
					memcpy(pOut, pRecv.pBuff + fi_start, fi_stop - fi_start);
					pOut[fi_stop - fi_start] = 0;
					return 0;
				}
			}
		}
				
//		if(point_num == 3)
//		{
//			while(fi < 0 && times < sec * 1000)
//			{
//				mew_m26_DelayMS_Hook(1);
//				times ++;
//				fi = mew_buff_SearchFrom(pRecv, fi_add, p_r_n);
//			}
//			if(fi >= 0)
//			{
//				fi_stop = fi;
//				
//			}
//		}
		
//		times = 0;
//		*(recvStream.pLength) = 0;
//		
//		pRecv.pBuff = recvStream.pBuff;		
//		pRecv.Length = *(recvStream.pLength);
//		
//		fi = -1;
//		while(fi < 0 && times < sec * 1000)
//		{
//			mew_m26_DelayMS_Hook(1);
//			times ++;
//			pRecv.Length = *(recvStream.pLength);
//			fi = mew_buff_Search(pRecv, p_r_n);
//		}
//		
//		if(fi >= 0)
//		{
//			memcpy(pOut, pRecv.pBuff, fi);
//			pOut[fi] = 0;
//		}
		
		//break;
	}
	//mwM26DelayMSHook(5000);
	//memset(RXHandle.hBuff, 0, *(RXHandle.hLength));
	*(recvStream.pLength) = 0;	
	return -1;
}

static int8_t AT_QIOPEN(uint8_t ch, char *pIP, uint16_t pPORT, uint32_t sec)
{
	char req[64];
	char resp[32];
	char resp2[32];
	
	int16_t fi;
	uint32_t times;
	
	mew_buff_Handle_t pReq;
	mew_buff_Handle_t pResp;
	mew_buff_Handle_t pRecv;	
	mew_buff_Handle_t pResp2;
	
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
    mew_m26_SendBuff_Hook(pReq.pBuff, pReq.Length);
    
		mew_m26_DelayMS_Hook(AT_CMD_TIMEOUT_MSEC);
		
		pRecv.pBuff = recvStream.pBuff;
		pRecv.Length = *(recvStream.pLength);
		fi = -1;

		while(times < sec * 1000)
		{
			mew_m26_DelayMS_Hook(1);
			times ++;
			pRecv.Length = *(recvStream.pLength);
			fi = mew_buff_Search(pRecv, pResp);
			if(fi >= 0)
			{
				*(recvStream.pLength) = 0;
				return 0;
			}
			fi = mew_buff_Search(pRecv, pResp2);
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

static int8_t AT_QISEND(uint8_t ch, uint8_t *buff, uint16_t len)
{
	char req[32];
	
	sprintf((char *)req, "AT+QISEND=%d,%d\r", ch, len);
	
	if(AT_X(req, "\r\r\n> ", 1000, 0) >= 0)
	{
		if(AT_X((char *)buff, "SEND OK", 10000, 0) >= 0)
		{
			return 0;
		}
	}	
	return -1;
}

static int16_t AT_QIRD(uint8_t ch, char *pIP, uint16_t pPORT, uint16_t readMax, uint32_t sec)
{  
	char req[20];
	char resp[64];
		
	int16_t fi;
	uint32_t times;
	
	char dlenBuff[6] = {0};
	
	uint8_t i;
	
	mew_buff_Handle_t pReq;
	mew_buff_Handle_t pResp;
	mew_buff_Handle_t pRecv;
	mew_buff_Handle_t pOK;
	mew_buff_Handle_t pERROR;
	
	mew_buff_Handle_t payload;
	
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
    mew_m26_SendBuff_Hook(pReq.pBuff, pReq.Length);
						
		pRecv.pBuff = recvStream.pBuff;
		pRecv.pLength = (recvStream.pLength);
		
		do
		{
			mew_m26_DelayMS_Hook(1);
			times ++;
			pRecv.Length = *pRecv.pLength;
			if(mew_buff_EndWith(pRecv, pOK))
			{
				break;
			}
			if(mew_buff_EndWith(pRecv, pERROR))
			{
				break;
			}
		}
		while(times < AT_CMD_TIMEOUT_MSEC);
		
		sprintf(resp, "AT+QIRD=0,1,%d,%d\r\r\nOK\r\n", ch, readMax);
		pResp.pBuff = (uint8_t *)resp;
		pResp.Length = strlen((char *)pResp.pBuff);
		fi = mew_buff_Search(pRecv, pResp);
		if(fi >= 0)
		{			
			*(recvStream.pLength) = 0;	
			return 0;
		}
		else
		{
			sprintf(resp, "AT+QIRD=0,1,%d,%d\r\r\nERROR\r\n", ch, readMax);
			pResp.pBuff = (uint8_t *)resp;
			pResp.Length = strlen((char *)pResp.pBuff);
			fi = mew_buff_Search(pRecv, pResp);
			if(fi >= 0)
			{
				*(recvStream.pLength) = 0;	
				return -2;
			}
			else
			{
				//sprintf(resp, "AT+QIRD=0,1,%d,%d\r\r\n+QIRD: %s:%d,TCP,", ch, readMax, pIP, pPORT);
				sprintf(resp, "+QIRD: %s:%d,TCP,", pIP, pPORT);
				pResp.pBuff = (uint8_t *)resp;
				pResp.Length = strlen((char *)pResp.pBuff);
				fi = mew_buff_Search(pRecv, pResp);	
				if(fi >= 0)
				{
					mew_m26_SocketRecvStart_Hook(ch);
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
					while((*(recvStream.pLength) < fi + pResp.Length + i + 2 + payload.Length + 6) && (times < AT_CMD_TIMEOUT_MSEC))
					{
						mew_m26_DelayMS_Hook(1);
						times ++;
					}
					if(mew_buff_OffsetWith(fi + pResp.Length + i + 2 + payload.Length, pRecv, pOK))
					{
						payload.pBuff = mew_m26_Malloc_Hook(payload.Length);
						if(payload.pBuff)
						{
							memcpy(payload.pBuff, fi + pRecv.pBuff + pResp.Length + i + 2, payload.Length);
							mew_m26_SocketRecvDone_Hook(ch, payload.pBuff, payload.Length);
							mew_m26_Free_Hook(payload.pBuff);
							*(recvStream.pLength) = 0;
							return payload.Length;
						}
						else
						{
							mew_m26_MallocErr_Hook();
						}
					}
					else
					{
						mew_m26_SocketRecvParseErr_Hook(ch);
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
	return -1;
}

//static int8_t GPSEnable(void)
//{
//	int8_t err_res;
//	err_res = AT_X("AT+QGNSSC=1\r", "AT+QGNSSC=1\r\r\nOK\r\n", AT_CMD_TIMEOUT_MSEC, 3);
////	if(err_res)
//	{
//		return err_res;
//	}
//}
//static int8_t GPSDisable(void)
//{
//	int8_t err_res;
//	err_res = AT_X("AT+QGNSSC=0\r", "AT+QGNSSC=1\r\r\nOK\r\n", AT_CMD_TIMEOUT_MSEC, 3);
////	if(err_res)
//	{
//		return err_res;
//	}
//}
static int8_t reset(void)
{
	int8_t err_res;
	err_res = AT_X("AT\r", "AT\r\r\nOK\r\n", AT_CMD_TIMEOUT_MSEC, 3);
	if(err_res)
	{
		return err_res;
	}
	err_res = AT_X("AT+QPOWD=1\r", "AT+QPOWD=1\r\r\n", AT_CMD_TIMEOUT_MSEC, 3);
	if(err_res)
	{
		return err_res;
	}
	return 0;
}

static int8_t GPRSConn(void)
{
	int8_t err_res;	
	err_res = AT_X("AT\r", "AT\r\r\nOK\r\n", AT_CMD_TIMEOUT_MSEC, 30);
	if(err_res)
	{
		return err_res;
	}
	err_res = AT_GSN();
	if(err_res)
	{
		return err_res;
	}
	err_res = AT_X("AT+QGNSSC=1\r", "AT+QGNSSC=1\r\r\nOK\r\n", AT_CMD_TIMEOUT_MSEC, 3);
	if(err_res)
	{
		return err_res;
	}
	err_res = AT_X("AT+CPIN?\r", "AT+CPIN?\r\r\n+CPIN: READY\r\n\r\nOK\r\n", AT_CMD_TIMEOUT_MSEC, 3);
	if(err_res)
	{
		return err_res;
	}
	err_res = AT_X("AT+CGATT=0\r", "AT+CGATT=0\r\r\nOK\r\n", AT_CMD_TIMEOUT_MSEC, 3);
	if(err_res)
	{
		return err_res;
	}
	err_res = AT_X("AT+CGATT=1\r", "\r\nOK\r\n", AT_CMD_TIMEOUT_MSEC, 200);
	if(err_res)
	{
		return err_res;
	}
	err_res = AT_X("AT+QIDNSIP=1\r", "AT+QIDNSIP=1\r\r\nOK\r\n", AT_CMD_TIMEOUT_MSEC, 3);
	if(err_res)
	{
		return err_res;
	}	
	err_res = AT_X("AT+QINDI=1\r", "AT+QINDI=1\r\r\nOK\r\n", AT_CMD_TIMEOUT_MSEC, 3);
	if(err_res)
	{
		return err_res;
	}	
	err_res = AT_X("AT+QIMUX=1\r", "AT+QIMUX=1\r\r\nOK\r\n", AT_CMD_TIMEOUT_MSEC, 3);
	if(err_res)
	{
		return err_res;
	}
	err_res = AT_X("AT+QISDE=0\r", "AT+QISDE=0\r\r\nOK\r\n", AT_CMD_TIMEOUT_MSEC, 3);
	if(err_res)
	{
		return err_res;
	}

	
	return 0;
}

static int8_t socketConn(uint8_t ch)
{
	int8_t ret = AT_QIOPEN(ch, mew_m26.IP[ch], mew_m26.PORT[ch], 10);
	lastConnTime[ch] = *systicks;
	if(0<= ret)
	{
		lastCommTime[ch] = *systicks;
	}
	return ret;
}

static void socketPush(uint8_t ch, uint8_t *txbuff, uint16_t len)
{
	//if(sendStream.Length > 0)
	{
		memcpy(sendStream.pBuff, txbuff, len);		
		sendStream.Length = len;		
		sendingChannel = ch;
	}
}

static int8_t socketSend(uint8_t ch, uint8_t *buff, uint16_t len)
{
	int8_t ret = AT_QISEND(ch, buff, len);
	if(0 <= ret)
	{
		lastCommTime[ch] = *systicks;
	}	
	return ret;
}

static int16_t socketRecv(uint8_t ch)
{	
	int16_t ret = 0;
	isRecv = 1;
	ret = AT_QIRD(ch, mew_m26.IP[ch], mew_m26.PORT[ch], 256, 1000);
	isRecv = 0;
	if(ret >= 0)
	{
		lastReadTime[ch] = *systicks;
		if(ret > 0)
		{		
			lastCommTime[ch] = *systicks;
		}
	}		
	return ret;
}

static uint8_t is_recv(void)
{
	return isRecv;
}
//static int8_t GPSParse(char *info)
//{
//}
static void socket_Schedule_NoOS(void)
{
	uint8_t i;
	int8_t res;
	
	if(mew_m26.WorkState == M26_WS_RESETING)
	{		
		mew_m26_ResetStart_Hook();
		if(0 <= reset())
		{
			mew_m26.WorkState = M26_WS_RESET;
			mew_m26_Reset_Hook();
//			if(mew_m26.EnableLoc)
			{
//				GPSEnable();
			}
		}
	}		
	if(mew_m26.WorkState == M26_WS_GPRS_CONNECTING)
	{
		if(0 <= GPRSConn())
		{			
			mew_m26_GPRSConnDone_Hook();
			mew_m26.WorkState = M26_WS_GPRS_CONNECTED;
		}
		else
		{
			mew_m26_GPRSConnErr_Hook();
			mew_m26.WorkState = M26_WS_RESETING;
		}
	}
	if(mew_m26.WorkState == M26_WS_TCP_CONNECTING)
	{		
		if(0 < AT_QIDNSGIP(mew_m26.ADDR[socketConnectingChannel], mew_m26.IP[socketConnectingChannel], DNSGIP_TIMEOUT_SEC))
		{
			mew_m26_SocketConnErr_Hook(socketConnectingChannel, -2);
			//return 0;
		}
		if(0 <= AT_CLOSE(socketConnectingChannel, AT_CMD_TIMEOUT_MSEC))
		{
			socketStateClr(socketConnectingChannel);
		}
		else
		{
			mew_m26_SocketConnErr_Hook(socketConnectingChannel, -3);
			//close fail
		}	
		res = socketConn(socketConnectingChannel);
		lastConnOpTime = *systicks;
		if(res>= 0)
		{
			socketStateSet(socketConnectingChannel);
			mew_m26_SocketConnDone_Hook(socketConnectingChannel);
			//gprs conn ok
		}
		else
		{
			mew_m26_SocketConnErr_Hook(socketConnectingChannel, -1);
		}
		mew_m26.WorkState = M26_WS_IDLE;
	}
	
	if(mew_m26.WorkState == M26_WS_RESET)
	{
		mew_m26.WorkState = M26_WS_GPRS_CONNECTING;
	}	
	if(mew_m26.WorkState == M26_WS_GPRS_CONNECTED)
	{		
		mew_m26.WorkState = M26_WS_IDLE;
	}
	if(mew_m26.WorkState == M26_WS_TCP_CONNECTED)
	{
		mew_m26.WorkState = M26_WS_IDLE;
	}
	if(mew_m26.WorkState == M26_WS_IDLE)
	{
		for(i = 0; i < CHANNEL_COUNT; i ++)
		{
			if(socketEnableGet(i))//通道激活
			{
				if(socketStateGet(i))//通道已连接
				{
					if(sendStream.Length > 0 && sendingChannel == i)//如果有数据待发送
					{
						mew_m26_SocketSendStart_Hook(i);
						if(0 > socketSend(i, sendStream.pBuff, sendStream.Length))
						{
							socketStateClr(i);
							mew_m26_SocketSendErr_Hook(i);
							mew_m26_SocketDisconn_Hook(i, -2);
						}
						else
						{
							mew_m26_SocketSendDone_Hook(i);
							sendStream.Length = 0;
						}
					}
					else//没有数据需要发送
					{
						//长时间没有数据交互，发送心跳包
						if(*systicks - lastCommTime[i] >= 1000 * mew_m26.HeartbeatInterval_Sec)
						{
							mew_m26_SocketHeartbeat_Hook(i);							
						}
						else//空闲
						{						
							//读数据
							if(*systicks - lastReadTime[i] >= mew_m26.ReadInterval_MilliSec)
							{
								if(0 > socketRecv(i))
								{
									socketStateClr(i);
									mew_m26_SocketRecvErr_Hook(i);
									mew_m26_SocketDisconn_Hook(i, -1);
								}
							}
							else
							{
								//什么事都没有
							}
						}
					}					
					//Locking = 0;
				}
				else//通道未连接
				{
					if(*systicks - lastConnOpTime >= 1000 * mew_m26.ConnectionInterval_Sec)
					{
						if(*systicks - lastConnTime[i] >= 1000 * mew_m26.ConnectionInterval_Sec)
						{
							//socketStateClr(i);
							socketConnectingChannel = i;
							mew_m26.WorkState = M26_WS_TCP_CONNECTING;
							
							break;
						}
					}
				}
			}
		}
	}
}

