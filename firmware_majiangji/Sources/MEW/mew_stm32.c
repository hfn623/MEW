#include "mew_stm32.h"
#include "stm32f10x.h"


static uint64_t delayStart = 0;

mew_stm32_Handle_t mew_stm32;

__weak void mew_stm32_UARTRecvByte_Hook(uint8_t port, uint8_t byte)
{
}

__weak void mew_stm32_UARTRecvDone_Hook(uint8_t port)
{
}

__weak void mew_stm32_SysTick_Hook(void)
{
}

__weak void mew_stm32_RCCInit_Hook(void)
{
}

__weak void mew_stm32_PINsInit_Hook(void)
{
}

void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_FLAG_Update) != RESET)  
	{
		TIM_ClearITPendingBit(TIM4, TIM_FLAG_Update);  		
	}
}
void SysTick_Handler(void)
{
	mew_stm32.Nowticks++;
}

void USART1_IRQHandler(void)
{
	uint8_t byte;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		byte = USART_ReceiveData(USART1);    
		mew_stm32_UARTRecvByte_Hook(1, byte);
	}
	if(USART_GetITStatus(USART1, USART_IT_IDLE) == SET)
	{
		USART_ReceiveData(USART1);
		mew_stm32_UARTRecvDone_Hook(1);
	}
}

void USART2_IRQHandler(void)
{
	uint8_t byte;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
	{
		byte = USART_ReceiveData(USART2);    
		mew_stm32_UARTRecvByte_Hook(2, byte);
	}
	if(USART_GetITStatus(USART2, USART_IT_IDLE) == SET)
	{
		USART_ReceiveData(USART2);
		mew_stm32_UARTRecvDone_Hook(2);
	}
}

void USART3_IRQHandler(void)
{
	uint8_t byte;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
	{
		byte = USART_ReceiveData(USART3);    
		mew_stm32_UARTRecvByte_Hook(3, byte);
	}
	if(USART_GetITStatus(USART3, USART_IT_IDLE) == SET)
	{
		USART_ReceiveData(USART3);
		mew_stm32_UARTRecvDone_Hook(3);
	}
}

static void mew_stm32_DelayMS(uint32_t span)
{
	delayStart = mew_stm32.Nowticks;
	while(mew_stm32.Nowticks - delayStart < span);
}




static void mew_stm32_PINsInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
  
  // pb10 = uart3 tx  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// PB11 = UART3 RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// PA9 = uart1 tx  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// PA10 = UART1 RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		// PA2 = uart2 tx  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// PA3 = UART2 RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	// PA1 = ADC
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
	
	// PA4 = DAC
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
	
	
	
	
	
	
	// custom define

//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//  GPIO_Init(GPIOA, &GPIO_InitStructure); 

	mew_stm32_PINsInit_Hook();	
}

static void mew_stm32_RCCInit()
{
	NVIC_InitTypeDef NVIC_InitStructure;	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	//	NVIC_SetPriority(SysTick_IRQn, 10);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);  	
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure ); 
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );

	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );
	
	
//	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
//	NVIC_Init(&NVIC_InitStructure);  

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	mew_stm32_RCCInit_Hook();
}

//static void mew_stm32_TIM4Init(void)
//{
//	TIM_TimeBaseInitTypeDef TIM_TBIS; 

//	TIM_DeInit(TIM4);  
//	TIM_TBIS.TIM_Period = 2;  
//	TIM_TBIS.TIM_Prescaler = 36000 - 1;  
//	TIM_TBIS.TIM_CounterMode = TIM_CounterMode_Up;  
//	TIM_TimeBaseInit(TIM4, &TIM_TBIS);  
//	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);  
//	TIM_Cmd(TIM4, ENABLE);  
//}

static void mew_stm32_SystickInit(void)
{
	mew_stm32.Nowticks = 0;
  if(SysTick_Config(SystemCoreClock / 1000))
  { 
    /* Capture error */ 
    while (1);
  }
}

static void mew_stm32_ADC1Init(void)
{
	ADC_InitTypeDef ADC_InitStructure; 

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

  
	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	
	ADC_ResetCalibration(ADC1);	//使能复位校准  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	
	ADC_StartCalibration(ADC1);	 //开启AD校准
 
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
}

static void mew_stm32_UART1Init(uint32_t speed)
{
	USART_InitTypeDef USART_InitStructure;  

	USART_InitStructure.USART_BaudRate = speed;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init( USART1, & USART_InitStructure );
  
	USART_Cmd( USART1 , ENABLE );
  
	USART_ITConfig(USART1, USART_IT_IDLE , ENABLE);
	USART_ITConfig(USART1, USART_IT_RXNE , ENABLE);
}

static void mew_stm32_UART2Init(uint32_t speed)
{
	USART_InitTypeDef USART_InitStructure;  

	USART_InitStructure.USART_BaudRate = speed;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init( USART2, & USART_InitStructure );
  
	USART_Cmd( USART2 , ENABLE );
  
	USART_ITConfig(USART2, USART_IT_IDLE , ENABLE);
	USART_ITConfig(USART2, USART_IT_RXNE , ENABLE);
}

static void mew_stm32_UART3Init(uint32_t speed)
{
	USART_InitTypeDef USART_InitStructure;
	
	USART_InitStructure.USART_BaudRate = speed;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init( USART3, & USART_InitStructure );

	USART_Cmd( USART3 , ENABLE ); 
  
	USART_ITConfig(USART3, USART_IT_IDLE , ENABLE);
	USART_ITConfig(USART3, USART_IT_RXNE , ENABLE);
}

static void mew_stm32_UART1SendByte(uint8_t byte)
{
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) != SET);
	USART_SendData(USART1, byte);
}

static void mew_stm32_UART2SendByte(uint8_t byte)
{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) != SET);
	USART_SendData(USART2, byte);
}

static void mew_stm32_UART3SendByte(uint8_t byte)
{
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) != SET);
	USART_SendData(USART3, byte);
}

static void mew_stm32_UARTSendByte(uint8_t port, uint8_t byte)
{
	if(port == 1)
	{
		mew_stm32_UART1SendByte(byte);
	}
	else if(port == 2)
	{
		mew_stm32_UART2SendByte(byte);
	}
	else if(port == 3)
	{
		mew_stm32_UART3SendByte(byte);
	}
}

static void mew_stm32_UARTSendString(uint8_t port, char *str)
{
	while(*str != 0)
	{
		mew_stm32_UARTSendByte(port, *str++);
	}
}

static void mew_stm32_DACWrite(uint16_t val)
{
}

static uint16_t mew_stm32_ADCRead(void)
{
	uint16_t val;
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	 
	//delay_ms(10);
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	val = ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
	
	return val;
}

static void mew_stm32_PortAWrite(uint8_t pin, uint8_t state)
{
	pin &= 0x0f;
	GPIO_WriteBit(GPIOA, 1<< pin, (BitAction) state);
}

static uint8_t mew_stm32_PortARead(uint8_t pin)
{
	pin &= 0x0f;
	return GPIO_ReadInputDataBit(GPIOA, 1<< pin);
}

static void mew_stm32_PortBWrite(uint8_t pin, uint8_t state)
{
	pin &= 0x0f;
	GPIO_WriteBit(GPIOB, 1<< pin, (BitAction)state);
}

static uint8_t mew_stm32_PortBRead(uint8_t pin)
{
	pin &= 0x0f;
	return GPIO_ReadInputDataBit(GPIOB, 1<< pin);
}

static void mew_stm32_PortCWrite(uint8_t pin, uint8_t state)
{
	pin &= 0x0f;
	GPIO_WriteBit(GPIOC, 1<< pin, (BitAction)state);
}

static uint8_t mew_stm32_PortCRead(uint8_t pin)
{
	pin &= 0x0f;
	return GPIO_ReadInputDataBit(GPIOC, 1<< pin);
}

void mew_stm32_Init(uint32_t uart1speed, uint32_t uart2speed, uint32_t uart3speed)
{
	mew_stm32.DelayMS = mew_stm32_DelayMS;

	mew_stm32.UARTSendByte = mew_stm32_UARTSendByte;
	mew_stm32.UARTSendString = mew_stm32_UARTSendString;
	
	mew_stm32.DACWrite = mew_stm32_DACWrite;
	mew_stm32.ADCRead = mew_stm32_ADCRead;

	mew_stm32.PortAWrite = mew_stm32_PortAWrite;
	mew_stm32.PortBWrite = mew_stm32_PortBWrite;
	mew_stm32.PortCWrite = mew_stm32_PortCWrite;
	
	mew_stm32.PortARead = mew_stm32_PortARead;
	mew_stm32.PortBRead = mew_stm32_PortBRead;
	mew_stm32.PortCRead = mew_stm32_PortCRead;
	
	mew_stm32_RCCInit();
	mew_stm32_PINsInit();
	
	mew_stm32_UART1Init(uart1speed);
//	mew_stm32_UART2Init(uart2speed);
//	mew_stm32_UART3Init(uart3speed);	
	mew_stm32_ADC1Init();
	mew_stm32_SystickInit();
	
}
