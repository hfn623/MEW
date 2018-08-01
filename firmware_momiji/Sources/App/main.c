//cup=1; lvl1=180s, lvl2=, lvl3=, lvl4=, lvl5=110s
//cup=2;
#include "stm32f10x.h"
#include "mwBoard.h"
#include "mwM26.h"
#include "lcd19264.h"
#include "cJSON.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#define		SELECTED_GATE		3000

uint8_t lvl, cup, lvl_bak, cup_bak;
cJSON_Hooks cjson_hooks;
uint8_t mwM26RXBuff[512];
uint16_t mwM26RXCnt, url_len, req_len;
uint8_t mwM26httpresp[256];
uint8_t mwM26httpreq[256];
cJSON *JSONresp;
cJSON *status, *msg, *data, *surplus, *possible;
double runTime, totleTime;

double pos;

uint64_t runStart;
uint8_t pass = 0 ;
//uint8_t bai_mi_zhuan_yong = 0;

//uint8_t runtime[5][5]={
//	{180,110,80,50,90},
//	{230,150,90,60,110},
//	{330,200,105,66,140},
//	{390,250,125,76,170},
//	{470,300,145,86,210},
//};
uint32_t runtime[5][5]={
	{180, 230, 330, 390, 470},
	{110, 150, 200, 250, 300},
	{80, 90, 105, 125, 145},
	{50, 60, 66, 76, 86},
	{90, 110, 140, 170,210},
};

const uint8_t key1[8] = {3,7,0,2,1,1,1,9};

const char *download_url = "http://api.hljalibaba.com/Home/Data/getData";
const char *upload_url = "http://api.hljalibaba.com/Home/Data/uploadData";

void mwM26DelayMSHook(uint32_t timespan)
{
	mwBoard.DelayMS(timespan);
}

void mwBoardCOM1RecvByteHook(uint8_t byte)
{
	//mwBoard.COM2SendByte(byte);
}

void mwBoardCOM2RecvByteHook(uint8_t byte)
{
	mwM26RXBuff[mwM26RXCnt ++] = byte;
	//mwBoard.COM1SendByte(byte);
}

void mwBoardCOM2RecvStopHook(void)
{
	if(memcmp(mwM26RXBuff, key1, 8) == 0)
	{
		pass = 1;
	}
}

void mwM26SendHook(mwBuffHandle_t pack)
{
	uint16_t i;
	for(i = 0; i < pack.Length; i ++)
	{
		mwBoard.COM2SendByte(pack.pBuff[i]);
	}
}

void showInit(void)
{
	//LCD_ShowHZ12String(4 + 12 *  4 + 4 * 0, 3, "初始化中……");//1
	LCD_ShowPIC(welcome);
}

void showConn(void)
{
	LCD_ShowHZ12String(4 + 12 *  4 + 4 * 0, 3, "网络加载中……");//1
}

void showComm(void)
{
	LCD_ShowHZ12String(0 + 12 *  2 + 4 * 0, 3, "正在处理数据，请稍等……");//1
}

void showERR(void)
{
	LCD_ShowPIC(err);
}

void showMenuLvl(void)
{
	LCD_ShowHZ12String(4 + 12 *  0 + 4 * 0, 1, "精白米");//1
	LCD_ShowHZ12String(4 + 12 *  3 + 4 * 1, 1, "白米");//2
	LCD_ShowHZ12String(4 + 12 *  5 + 4 * 2, 1, "七分白");//4
	LCD_ShowHZ12String(4 + 12 *  8 + 4 * 3, 1, "五分白");//5
	LCD_ShowHZ12String(4 + 12 * 11 + 4 * 4, 1, "胚芽米");//3

}

void showWarn(void)
{
	LCD_ShowPIC(warn);
}

void showMenuCup(void)
{
	
	LCD_ShowHZ12String(12 + 12 * 0 + 12 * 0, 4, "一杯");//6
	LCD_ShowHZ12String(12 + 12 * 2 + 12 * 1, 4, "两杯");//7
	LCD_ShowHZ12String(12 + 12 * 4 + 12 * 2, 4, "三杯");//9
	LCD_ShowHZ12String(12 + 12 * 6 + 12 * 3, 4, "四杯");//10
	LCD_ShowHZ12String(12 + 12 * 8 + 12 * 4, 4, "五杯");//8
}

void showSelectLvl(void)
{
	if(lvl == 1)
	{
		LCD_ShowHZ12String(4 + 12 *  0 + 4 * 0, 1, "　　　");//1
	}
	if(lvl == 2)
	{
		LCD_ShowHZ12String(4 + 12 *  3 + 4 * 1, 1, "　　");//2
	}
	if(lvl == 3)
	{
		LCD_ShowHZ12String(4 + 12 *  5 + 4 * 2, 1, "　　　");//4
	}
	if(lvl == 4)
	{
		LCD_ShowHZ12String(4 + 12 *  8 + 4 * 3, 1, "　　　");//5
	}
	if(lvl == 5)
	{
		LCD_ShowHZ12String(4 + 12 * 11 + 4 * 4, 1, "　　　");//3
	}
	
}
void showSelectCup(void)
{

	
	if(cup == 1)
	{
		LCD_ShowHZ12String(12 + 12 * 0 + 12 * 0, 4, "　　");//6
	}
	if(cup == 2)
	{
		LCD_ShowHZ12String(12 + 12 * 2 + 12 * 1, 4, "　　");//7
	}
	if(cup == 3)
	{
		LCD_ShowHZ12String(12 + 12 * 4 + 12 * 2, 4, "　　");//9
	}
	if(cup == 4)
	{
		LCD_ShowHZ12String(12 + 12 * 6 + 12 * 3, 4, "　　");//10
	}
	if(cup == 5)
	{
		LCD_ShowHZ12String(12 + 12 * 8 + 12 * 4, 4, "　　");//8
	}
}

void initKeys(void)
{
	uint16_t adv;
	uint8_t i;
	uint32_t sum;
	
	sum = 0;
	for(i = 0; i < 10; i ++)
	{
		sum += mwBoard.ADCRead(6);
	}
	adv = sum / 10;
	while(adv > SELECTED_GATE)
	{		
		pressKey1();
		mwBoard.DelayMS(300);		
		sum = 0;
		for(i = 0; i < 10; i ++)
		{
			sum += mwBoard.ADCRead(6);
		}
		adv = sum / 10;
	}
	
	mwBoard.DelayMS(300);
	
	sum = 0;
	for(i = 0; i < 10; i ++)
	{
		sum += mwBoard.ADCRead(5);
	}
	adv = sum / 10;
	while(adv > SELECTED_GATE)
	{		
		pressKey2();
		mwBoard.DelayMS(300);
		sum = 0;
		for(i = 0; i < 10; i ++)
		{
			sum += mwBoard.ADCRead(5);
		}
		adv = sum / 10;		
	}
	
	mwBoard.DelayMS(300);
}
uint8_t getData(void)
{
	uint8_t ret;
	sprintf((char *)mwM26httpreq,"equipment_id=%s",mwM26.IMEI);
	req_len = strlen((char *)mwM26httpreq);
	url_len = strlen(download_url);
	ret = mwM26.HTTP_POST(download_url, url_len, mwM26httpreq, req_len, mwM26httpresp);
				
	JSONresp = cJSON_Parse((char *)mwM26httpresp);

	status = cJSON_GetObjectItem(JSONresp, "status");
	msg =  cJSON_GetObjectItem(JSONresp, "msg");
	data = cJSON_GetObjectItem(JSONresp, "data");
	surplus = cJSON_GetObjectItem(data, "surplus");
	possible =  cJSON_GetObjectItem(data, "possible");
	
	if(strcmp(msg->valuestring, "1000"))
	{
		ret = 0;
	}
	
	return ret;
}

uint8_t uploadData(uint8_t cnt)
{
	uint8_t ret;
	sprintf((char *)mwM26httpreq,"amount=%d&equipment_id=%s", cnt, mwM26.IMEI);
	req_len = strlen((char *)mwM26httpreq);
	url_len = strlen(upload_url);
	ret = mwM26.HTTP_POST(upload_url, url_len, mwM26httpreq, req_len, mwM26httpresp);
				
	JSONresp = cJSON_Parse((char *)mwM26httpresp);

	
	msg =  cJSON_GetObjectItem(JSONresp, "msg");
	
	if(strcmp(msg->valuestring, "2000"))
	{
		ret = 0;
	}
	
	return ret;
}

uint8_t key[8] = {0,6,2,3,2,0,1,5};

int main(void)
{	
	keyEnable = 0;
	boardInit();	
	
  cjson_hooks.malloc_fn = malloc;
  cjson_hooks.free_fn = free;
  cJSON_InitHooks(&cjson_hooks);
	
	
	mwM26Init(mwM26RXBuff, &mwM26RXCnt, &mwBoard.Nowticks);
	
	LCD_LED_Toggle(0);
	
	LCD_RST_Toggle(0);
	mwBoard.DelayMS(10);
  LCD_RST_Toggle(1);
	
	while(1)
	{
		if(pass)
		{
			mwBoard.DelayMS(100);
			mwBoard.COM2SendBuff(key, 8);
			break;
		}
	}	
	
  LCD_LED_Toggle(1);
  
  LCD_Power(1);	

	LCD_Clear(0);
	
	showInit();
	
	initKeys();	
	
	cup = 1;
	lvl = 1;
	
	while(1)
	{
		mwM26.Schedule_HTTP_NoOS();
//		if(mwM26.WorkState == M26_WS_RESETING)
//		{
//			LCD_Clear(0xaa);
//			mwBoard.DelayMS(1000);			
//		}	
//		if(mwM26.WorkState == M26_WS_RESET)
//		{
//			
//		}			
//		if(mwM26.WorkState == M26_WS_GPRS_CONNECTING)
//		{
//			showWarn();
//			mwBoard.DelayMS(1000);
//		}		
		if(mwM26.WorkState == M26_WS_GPRS_CONNECTED)
		{	
			LCD_Clear(0);			
			if(getData() == 0)
			{
				showERR();
				mwBoard.DelayMS(10000);
				NVIC_SystemReset();
			}
			if(possible->valueint == 0)
			{
				showERR();
				mwBoard.DelayMS(30000);
				NVIC_SystemReset();
			}
			keyEnable = 1;
		}
		if(mwM26.WorkState == M26_WS_IDLE)
		{
			if(SW1)
			{				
				pressKey1();
				mwBoard.DelayMS(80);
				
				showMenuLvl();
				
				SW1 = 0;
				
				lvl ++;
				if(lvl > 5)
				{
					lvl = 1;
				}
			}
			if(SW2)
			{				
				pressKey2();
				mwBoard.DelayMS(80);
				
				showMenuCup();
				
				SW2 = 0;
				
				cup ++;
				if(cup > 5)
				{
					cup = 1;
				}
			}
			if(SW3)
			{			
				keyEnable = 0;
//				pressKey3();
				mwBoard.DelayMS(80);
//				bai_mi_zhuan_yong = 1;
//				running = 1;
				SW3 = 0;
			}
			if(SW4)
			{
				keyEnable = 0;
				pressKey4();
				mwBoard.DelayMS(80);
				running = 1;
				SW4 = 0;
			}
			if(running == 0)
			{				
				if(mwBoard.Nowticks % 1000 == 0)
				{
					showMenuLvl();
				}
				if(mwBoard.Nowticks % 1000 == 600)
				{
					showSelectLvl();
				}
				if(mwBoard.Nowticks % 1000 == 200)
				{
					showMenuCup();
				}
				if(mwBoard.Nowticks % 1000 == 800)
				{
					showSelectCup();
				}
				if(mwBoard.Nowticks % 1000 == 400)
				{
					keyEnable = 1;
				}
			}
			else
			{	
//				if(bai_mi_zhuan_yong)
//				{
//					lvl_bak = lvl;
//					cup_bak = cup;
//				}
				
				showWarn();
				
				
				runStart = mwBoard.Nowticks;				
				
				runTime = 0;
				totleTime = (runtime[lvl - 1][cup - 1] * 1000);
				while(runTime < totleTime)
				{
					runTime = mwBoard.Nowticks - runStart;
					pos = runTime * 192.0 / totleTime;
					//pos = percent * 192.0;
					LCD_Write((uint8_t)pos, 7, 15);
					if(running == 0)
					{
						break;
					}
				}
				
				uploadData(cup);
				
//				if(bai_mi_zhuan_yong)
//				{
//					lvl = lvl_bak;
//					cup = cup_bak;
//					bai_mi_zhuan_yong = 0;
//				}
				
				running = 0;
				LCD_Clear(0);								
			}
		}
	}
}
