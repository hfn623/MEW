#include "stm32f10x.h"

#include "mew_board.h"
#include "mew_stm32.h"
#include "mew_m26.h"
#include "mew_nmea.h"

#include <stdio.h>
#include <string.h>

static char tmpstr[1024];

extern uint16_t gnss_rx_bufflen;
extern uint8_t gnss_rx_buff[GNSS_RX_BUFF_COUNT];
extern mew_GNGLL_Data gngll_data;
extern uint8_t buffed_frames;

int main(void)
{
	mew_board_Init();
	
	sprintf(mew_m26.ADDR[0], "www.boryworks.com");
	mew_m26.PORT[0] = 9000;
	
	mew_m26.SocketEnable(0);
	
	mew_board.LED_NS(1);
	mew_board.MC20_PK(0);
	
	while(1)
	{
		mew_m26.Socket_Schedule_NoOS();
		
		if(buffed_frames > 0)
		{
			if(0 == mew_NMEA_Parse_GNGLL((char *)gnss_rx_buff, &gngll_data))
			{
				mew_board.LED_STA(1);
			}
			else
			{
				mew_board.LED_STA(0);
			}
			gnss_rx_bufflen = 0;
			buffed_frames = 0;
		}
	}
}
void mew_m26_Reset_Hook(void)
{
	mew_board.LED_TX(1);
	mew_board.LED_RX(1);
	mew_board.LED_STA(1);
}

void mew_m26_GPRSConnDone_Hook(void)
{
	mew_board.LED_STA(0);
}

void mew_m26_SocketConnDone_Hook(uint8_t ch)
{
	if(ch == 0)
	{
		mew_board.LED_TX(0);
		mew_board.LED_RX(0);
		mew_m26.SocketSend(ch, (uint8_t *)"mc20_conn_done\n", 15);
	}
}
void mew_m26_SocketSendStart_Hook(uint8_t ch)
{
	mew_board.LED_RX(1);
}
void mew_m26_SocketSendDone_Hook(uint8_t ch)
{
	mew_board.LED_RX(0);
}
void mew_m26_SocketSendErr_Hook(uint8_t ch)
{
	mew_board.LED_RX(1);
}
void mew_m26_SocketHeartbeat_Hook(uint8_t ch)
{
	sprintf(tmpstr, "LOC = %d, %04f, %04f\n", gngll_data.Vaild, gngll_data.Latitude, gngll_data.Longitude);
	mew_m26.SocketSend(ch, (uint8_t *)tmpstr, strlen(tmpstr));
}
void mew_m26_SocketRecvStart_Hook(uint8_t ch)
{
	mew_board.LED_RX(1);
	
	sprintf(tmpstr, "lib event: socket%d recv start\n", ch);
	mew_stm32.UARTSendString(1, tmpstr);
}
void mew_m26_SocketRecvDone_Hook(uint8_t ch, uint8_t *buff, uint16_t len)
{	
	mew_board.LED_RX(0);
	
	sprintf(tmpstr, "lib event: socket%d recv done\n", ch);
	mew_stm32.UARTSendString(1, tmpstr);
}
void mew_m26_SocketRecvErr_Hook(uint8_t ch)
{
	mew_board.LED_RX(1);
	
	sprintf(tmpstr, "lib event: socket%d recv err\n", ch);
	mew_stm32.UARTSendString(1, tmpstr);
}
void mew_m26_SocketDisconn_Hook(uint8_t ch, int8_t reason)
{
	mew_board.LED_RX(1);
	mew_board.LED_TX(1);
	mew_board.LED_STA(0);
	
	sprintf(tmpstr, "lib event: socket%d disconn, reason = %d\n", ch, reason);
	mew_stm32.UARTSendString(1, tmpstr);
}
