#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mew_board.h"
#include "stm32f10x.h"
#include "wizchip_conf.h"
#include "freertos.h"
#include "semphr.h"
#include "task.h"
#include "socket.h"

mew_board_handle_t mew_board;
net_parms_handle_t net_parms;
SemaphoreHandle_t sem_udp_recv;
QueueHandle_t que_rly;

void W5500_IRQ_HANDLER()
{
	sockint_kind sik;
	if(EXTI_GetITStatus(EXTI_Line0) != RESET)
	{
		if(SOCK_OK != ctlsocket(SOCK_OF_UDP, CS_GET_INTERRUPT, &sik))
		{
		}
		if(sik == SIK_RECEIVED)
		{
			xSemaphoreGiveFromISR(sem_udp_recv, NULL);
		}
		EXTI_ClearITPendingBit(EXTI_Line0);
	} 
}

//dead loop delay
static void mew_board_dl_dly_us(uint32_t ts)
{
	uint32_t i = 0;
	for(i = 0; i < ts; i++)
	{
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	}
}

static void mew_board_dl_dly_ms(uint32_t ts)
{
	uint32_t i = 0;
	for(i = 0; i < ts; i++)
	{
		mew_board_dl_dly_us(1000);
	}
}

static void mew_w5500_rst(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_1);
	GPIO_ResetBits(GPIOB, GPIO_Pin_1);
	mew_board_dl_dly_ms(1);
	GPIO_SetBits(GPIOB, GPIO_Pin_1);
}

static void mew_w5500_spi_cs(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
}

static void mew_w5500_spi_decs(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
}

static void mew_w5500_spi_wr_byte(uint8_t wb)
{
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);         
	SPI_I2S_SendData(SPI1, wb);				
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);				
	SPI_I2S_ReceiveData(SPI1);
}

static uint8_t mew_w5500_spi_rd_byte(void)
{
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);         
	SPI_I2S_SendData(SPI1, 0xff);				
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);				
	return SPI_I2S_ReceiveData(SPI1);
}

static void mew_w5500_net_params_init(void)
{
	uint8_t chipid[6];
	wiz_NetInfo gWIZNETINFO;
		
	uint8_t mac[6]={0x00,0x08,0xdc,0x11,0x11,0x11}; ///< Source Mac Address
	uint8_t ip[4]={192,168,1,150}; ///< Source IP Address
	uint8_t sn[4]={255,255,255,0}; ///< Subnet Mask
	uint8_t gw[4]={192,168,1,1}; ///< Gateway IP Address
	uint8_t dns[4]={8,8,8,8}; ///< DNS server IP Address

	memcpy(gWIZNETINFO.ip, ip, 4);
	memcpy(gWIZNETINFO.sn, sn, 4);
	memcpy(gWIZNETINFO.gw, gw, 4);
	memcpy(gWIZNETINFO.mac, mac, 6);
	memcpy(gWIZNETINFO.dns, dns, 4);
	gWIZNETINFO.dhcp = NETINFO_STATIC; //< 1 - Static, 2 - DHCP
	ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);

	ctlnetwork(CN_GET_NETINFO, (void*)&gWIZNETINFO);
	// Display Network Information
	ctlwizchip(CW_GET_ID,(void*)chipid);
	//printf("\r\n=== %s NET CONF ===\r\n",(char*)chipid);
	//printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",gWIZNETINFO.mac[0],gWIZNETINFO.mac[1],gWIZNETINFO.mac[2],
	//gWIZNETINFO.mac[3],gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);
	//printf("SIP: %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);
	//printf("GAR: %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0],gWIZNETINFO.gw[1],gWIZNETINFO.gw[2],gWIZNETINFO.gw[3]);
	//printf("SUB: %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0],gWIZNETINFO.sn[1],gWIZNETINFO.sn[2],gWIZNETINFO.sn[3]);
	//printf("DNS: %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0],gWIZNETINFO.dns[1],gWIZNETINFO.dns[2],gWIZNETINFO.dns[3]);
	//printf("======================\r\n");

	wizchip_init(NULL, NULL);
	

	wiz_NetTimeout w_NetTimeout;
	w_NetTimeout.retry_cnt = 50;
	w_NetTimeout.time_100us = 1000;
	wizchip_settimeout(&w_NetTimeout);
	
	
	wizchip_setinterruptmask(IK_SOCK_ALL);
}

static void mew_w5500_init(void)
{
	reg_wizchip_cris_cbfunc(NULL, NULL); // 注册临界区函数
	reg_wizchip_cs_cbfunc(mew_w5500_spi_cs, mew_w5500_spi_decs);// 注册片选函数
	reg_wizchip_spi_cbfunc(mew_w5500_spi_rd_byte, mew_w5500_spi_wr_byte); // 注册读写函数
	
	
}

static void mew_board_stm32_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	SPI_InitTypeDef   SPI_InitStructure;	
	EXTI_InitTypeDef EXTI_InitStructure;//定义初始化结构体	
	NVIC_InitTypeDef NVIC_InitStructure;//定义结构体

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//4bit抢占优先级
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn; //使能外部中断所在的通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0xa; //抢占优先级 10
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道 
	NVIC_Init(&NVIC_InitStructure); //根据结构体信息进行优先级初始化	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_4 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI1, &SPI_InitStructure);
	SPI_Cmd(SPI1, ENABLE);
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);	

	EXTI_InitStructure.EXTI_Line = EXTI_Line0; //中断线的标号 取值范围为EXTI_Line0~EXTI_Line15
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断模式，可选值为中断 EXTI_Mode_Interrupt 和事件 EXTI_Mode_Event。
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//触发方式，可以是下降沿触发 EXTI_Trigger_Falling，上升沿触发 EXTI_Trigger_Rising，或者任意电平（上升沿和下降沿）触发EXTI_Trigger_Rising_Falling
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);//根据结构体信息进行初始化


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//GPIO_ResetBits(GPIOB, GPIO_Pin_5);
}

static void mew_board_rly_rst()
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_2);
	GPIO_ResetBits(GPIOA, GPIO_Pin_3);
	GPIO_ResetBits(GPIOA, GPIO_Pin_0);
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	GPIO_ResetBits(GPIOB, GPIO_Pin_8);
	GPIO_ResetBits(GPIOB, GPIO_Pin_9);
	GPIO_ResetBits(GPIOC, GPIO_Pin_14);
	GPIO_ResetBits(GPIOC, GPIO_Pin_15);
}

static void mew_board_rly_wr(uint8_t port, uint8_t state)
{
	switch(port)
	{
		case 0:
			if(state)
			{
				GPIO_ResetBits(GPIOA, GPIO_Pin_2);
				GPIO_SetBits(GPIOA, GPIO_Pin_3);
			}
			else
			{
				GPIO_ResetBits(GPIOA, GPIO_Pin_3);
				GPIO_SetBits(GPIOA, GPIO_Pin_2);
			}
			break;
		case 1:
			if(state)
			{
				GPIO_ResetBits(GPIOA, GPIO_Pin_0);
				GPIO_SetBits(GPIOA, GPIO_Pin_1);
			}
			else
			{
				GPIO_ResetBits(GPIOA, GPIO_Pin_1);
				GPIO_SetBits(GPIOA, GPIO_Pin_0);
			}
			break;
		case 2:
			if(state)
			{
				GPIO_ResetBits(GPIOB, GPIO_Pin_8);
				GPIO_SetBits(GPIOB, GPIO_Pin_9);
			}
			else
			{
				GPIO_ResetBits(GPIOB, GPIO_Pin_9);
				GPIO_SetBits(GPIOB, GPIO_Pin_8);
			}
			break;
		case 3:
			if(state)
			{
				GPIO_ResetBits(GPIOC, GPIO_Pin_14);
				GPIO_SetBits(GPIOC, GPIO_Pin_15);
			}
			else
			{
				GPIO_ResetBits(GPIOC, GPIO_Pin_15);
				GPIO_SetBits(GPIOC, GPIO_Pin_14);
			}
			break;
		default:
			break;
	}
}

static void mew_board_led_d5(uint8_t state)
{
	if(state)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_15);		
	}
	else
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_15);
	}
}

static void mew_board_led_d6(uint8_t state)
{
	if(state)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_3);		
	}
	else
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_3);
	}
}

static uint8_t mew_board_op_rd(uint8_t port)
{
	switch(port)
	{
		case 0:
			return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);
		case 1:
			return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14);
		case 2:
			return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13);
		case 3:
			return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);
	}
	return 0;
}

void mew_board_init()
{
	mew_board.rly_wr = mew_board_rly_wr;
	mew_board.rly_rst = mew_board_rly_rst;
	mew_board.op_rd = mew_board_op_rd;
	
	mew_board.led_d5 = mew_board_led_d5;
	mew_board.led_d6 = mew_board_led_d6;

	mew_board.dl_dly_ms = mew_board_dl_dly_ms;
	mew_board.dl_dly_us = mew_board_dl_dly_us;
	
	mew_board_stm32_init();	
	mew_w5500_rst();	
	mew_board_dl_dly_ms(1);	
	mew_w5500_init();		
	mew_w5500_net_params_init();
	
	if(SysTick_Config(SystemCoreClock / 1000))
	{
		while(1);
	}
}

void mew_m24128_dl_dly_ms_5_hook()
{
	mew_board.dl_dly_ms(5);
}

void mew_iic_sda_mode_output_hook(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void mew_iic_sda_mode_input_hook(void)	
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void mew_m24128_wc_wr_h_hook(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_5);
}

void mew_m24128_wc_wr_l_hook(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_5);
}

void mew_iic_scl_wr_h_hook(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_6);
}

void mew_iic_scl_wr_l_hook(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_6);
}

void mew_iic_sda_wr_h_hook(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_7);
}

void mew_iic_sda_wr_l_hook(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_7);
}

uint8_t mew_iic_sda_rd_hook(void)
{
	return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)? 1 : 0;
}

