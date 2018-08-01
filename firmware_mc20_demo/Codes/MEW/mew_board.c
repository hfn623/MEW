#include "mew_board.h"
#include "mew_stm32.h"
#include "mew_m26.h"
#include "stm32f10x.h"
#include "stdio.h"

#define GPRS_TX_BUFF_COUNT 2048
#define GPRS_RX_BUFF_COUNT 2048
#define GNSS_RX_BUFF_COUNT 2048

uint8_t debug = 1;

mew_board_Handle_t mew_board;

static uint8_t gprs_tx_buff[GPRS_TX_BUFF_COUNT];
static uint8_t gprs_rx_buff[GPRS_RX_BUFF_COUNT];
static uint8_t gnss_rx_buff[GNSS_RX_BUFF_COUNT];

static uint16_t gprs_tx_bufflen;
static uint16_t gprs_rx_bufflen;
static uint16_t gnss_rx_bufflen;

void mew_stm32_PINsInit_Hook(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// MDTR
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
	
	// PA12 = MRI
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// PK
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

	// PB9 = IO
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);  

	// BUSY
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
	
	// LED TXD
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  

	// LED RXD
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  

	// LED STA
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure); 

	// net led enable
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
}
static void led_sta(uint8_t state)
{
	mew_stm32.PortBWrite(0, state);
}
static void led_ns(uint8_t state)
{
	mew_stm32.PortBWrite(1, state);
}
static void led_tx(uint8_t state)
{
	mew_stm32.PortAWrite(7, state);
}
static void led_rx(uint8_t state)
{
	mew_stm32.PortAWrite(6, state);
}
static void pin_io(uint8_t state)
{
	mew_stm32.PortBWrite(9, state);
}
static void pin_busy(uint8_t state)
{
	mew_stm32.PortAWrite(5, state);
}
static void mc20_pk(uint8_t state)
{
	mew_stm32.PortBWrite(8, state);
}
static void mc20_dtr(uint8_t state)
{
	mew_stm32.PortAWrite(11, state);
}

void mew_board_Init(void)
{
	mew_board.LED_NS = led_ns;
	mew_board.LED_RX = led_rx;
	mew_board.LED_TX = led_tx;
	mew_board.LED_STA = led_sta;
	mew_board.PIN_IO = pin_io;
	mew_board.PIN_BUSY = pin_busy;
	mew_board.MC20_PK = mc20_pk;
	mew_board.MC20_DTR = mc20_dtr;
	
	mew_stm32_Init(115200, 115200, 115200);
	
	mew_board.MC20_PK(1);
		
	mew_m26_Init(
	gprs_tx_buff, &gprs_tx_bufflen,
	gprs_rx_buff, &gprs_rx_bufflen,
	&mew_stm32.Nowticks);
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
		mew_stm32.UARTSendByte(3, buff[i]);
	}
}
void mew_stm32_UARTRecvDone_Hook(uint8_t port)
{
//	uint16_t i;
	if(port == 2)
	{
		//缓存内容全部发给用户
//		for(i = 0; i < gnss_rx_bufflen; i ++)
//		{
//			mew_stm32.UARTSendByte(1, gnss_rx_buff[i]);
//		}
		gnss_rx_bufflen = 0;
	}
}
void mew_stm32_UARTRecvByte_Hook(uint8_t port, uint8_t byte)
{
	//串口1是用户串口
	if(port == 1)
	{
		//直接转发给GPRS部分
		mew_stm32.UARTSendByte(3, byte);
	}
	//串口2连接GNSS模块
	if(port == 2)
	{
		//一旦符合条件缓存
		if(gnss_rx_bufflen < GNSS_RX_BUFF_COUNT)
		{
			gnss_rx_buff[gnss_rx_bufflen ++]= byte;
		}
//		mew_stm32.UARTSendByte(1, byte);
	}
	//串口3连接GPRS模块
	if(port == 3)
	{
		if(gprs_rx_bufflen < GPRS_RX_BUFF_COUNT)
		{
			gprs_rx_buff[gprs_rx_bufflen ++]= byte;
		}
		if(debug && mew_m26.IsReceiving() == 0)
		{
			mew_stm32.UARTSendByte(1, byte);
		}
	}
}
////////////////////////////////////////////////////////////////
//用户级抽象钩子函数
////////////////////////////////////////////////////////////////
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

void mew_m26_SocketHeartbeat_Hook(uint8_t ch)
{
	mew_m26.SocketSend(ch, (uint8_t *)"mc20_hb\n", 8);
}

void mew_m26_SocketDisconn_Hook(uint8_t ch, int8_t reason)
{
	if(ch == 0)
	{
		mew_board.LED_TX(1);
		mew_board.LED_RX(1);
	}
}
