
#include "bsp.h"

#define dly_tim 50

void delay_ms_rough(uint32_t time)
{
//	dly_tmr.Set(sys_tmr);
//	while(sys_tmr.GetTotalMS()-dly_tmr.GetTotalMS()<time);
   int i=0;   
   while( time -- ) 
   { 
      i = 8000;  //???? 
      while( i -- ) ;     
   } 
}

void UART1_Init( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init(GPIOA, & GPIO_InitStructure);
	
	GPIO_PinAFConfig( GPIOA , GPIO_PinSource9 , GPIO_AF_1 ); 
  GPIO_PinAFConfig( GPIOA , GPIO_PinSource10 , GPIO_AF_1 );
	
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init( & NVIC_InitStructure );
	
	USART_ITConfig( USART1 , USART_IT_RXNE , ENABLE );
	USART_ITConfig( USART1 , USART_IT_IDLE , ENABLE );		

	USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init( USART1 , & USART_InitStructure );
	
	USART_Cmd( USART1 , ENABLE );
}

void UART1_SendByte( uint8_t data )
{
	while( USART_GetFlagStatus( USART1 , USART_FLAG_TXE ) == RESET );
	USART_SendData( USART1 , data );
}

void UART1_SendBuf( uint8_t * pBuf , uint16_t len )
{
	uint16_t i;
	for( i = 0 ; i < len ; i ++ )
	{
		UART1_SendByte( pBuf[ i ] );
	}
}

void UART1_SendStr( char * pStr )
{
	for( ; * pStr ; )
	{
		UART1_SendByte( * pStr ++ );
	}
}

void UART2_Init( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init(GPIOA, & GPIO_InitStructure);
	
	GPIO_PinAFConfig( GPIOA , GPIO_PinSource2 , GPIO_AF_1 ); 
  GPIO_PinAFConfig( GPIOA , GPIO_PinSource3 , GPIO_AF_1 );
	
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init( & NVIC_InitStructure );
	
	USART_ITConfig( USART2 , USART_IT_RXNE , ENABLE );
	USART_ITConfig( USART2 , USART_IT_IDLE , ENABLE );		

	USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init( USART2 , & USART_InitStructure );
	
	USART_Cmd( USART2 , ENABLE );
}

void UART2_SendByte( uint8_t data )
{
	while( USART_GetFlagStatus( USART2 , USART_FLAG_TXE ) == RESET );
	USART_SendData( USART2 , data );
}

void UART2_SendBuf( uint8_t * pBuf , uint16_t len )
{
	uint16_t i;
	for( i = 0 ; i < len ; i ++ )
	{
		UART2_SendByte( pBuf[ i ] );
	}
}

void UART2_SendStr( char * pStr )
{
	for( ; * pStr ; )
	{
		UART2_SendByte( * pStr ++ );
	}
}

void RCC_Init( void )
{
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA , ENABLE );	
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOB , ENABLE );	
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 , ENABLE );
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2 , ENABLE );
}

void IO_Init( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_8;
  GPIO_Init( GPIOA, & GPIO_InitStructure ); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_12 | GPIO_Pin_11 | GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_Init( GPIOB, & GPIO_InitStructure ); 

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_5 | GPIO_Pin_4;
  GPIO_Init( GPIOB , & GPIO_InitStructure ); 	
}

int8_t OPTOISOLATOR_GET( uint8_t idx )
{
	if(idx == 0)
	{
		return GPIO_ReadInputDataBit( GPIOB , GPIO_Pin_7 );
	}
	else if(idx == 1)
	{
		return GPIO_ReadInputDataBit( GPIOB , GPIO_Pin_6 );
	}
	else if(idx == 2)
	{
		return GPIO_ReadInputDataBit( GPIOB , GPIO_Pin_5 );
	}
	else if(idx == 3)
	{
		return GPIO_ReadInputDataBit( GPIOB , GPIO_Pin_4 );
	}
	return -1;
}


void LED_SET(uint8_t idx, uint8_t stat)
{
	if(idx == 0)
	{
		if( stat )
		{
			GPIO_SetBits( GPIOB , GPIO_Pin_0 );
		}
		else
		{
			GPIO_ResetBits( GPIOB , GPIO_Pin_0 );
		}
	}
	else if(idx == 1)
	{
		if( stat )
		{
			GPIO_SetBits( GPIOB , GPIO_Pin_1 );
		}
		else
		{
			GPIO_ResetBits( GPIOB , GPIO_Pin_1 );
		}
	}
}

void RELAY_SET(uint8_t idx, uint8_t stat)
{
	if(idx == 0)
	{
		if( !stat )
		{
			GPIO_SetBits( GPIOA , GPIO_Pin_8 );
			delay_ms_rough(dly_tim);
			GPIO_ResetBits( GPIOA , GPIO_Pin_8 );
			delay_ms_rough(dly_tim);
		}
		else
		{
			GPIO_SetBits( GPIOB , GPIO_Pin_15 );
			delay_ms_rough(dly_tim);
			GPIO_ResetBits( GPIOB , GPIO_Pin_15 );
			delay_ms_rough(dly_tim);
		}
	}
	else if(idx == 1)
	{
		if( !stat )
		{
			GPIO_SetBits( GPIOA , GPIO_Pin_12 );
			delay_ms_rough(dly_tim);
			GPIO_ResetBits( GPIOA , GPIO_Pin_12 );
			delay_ms_rough(dly_tim);
		}
		else
		{
			GPIO_SetBits( GPIOA , GPIO_Pin_11 );
			delay_ms_rough(dly_tim);
			GPIO_ResetBits( GPIOA , GPIO_Pin_11 );
			delay_ms_rough(dly_tim);
			
		}
	}
	else if(idx == 2)
	{
		if( !stat )
		{
			GPIO_SetBits( GPIOB , GPIO_Pin_13 );
			delay_ms_rough(dly_tim);
			GPIO_ResetBits( GPIOB , GPIO_Pin_13 );
			delay_ms_rough(dly_tim);
		}
		else
		{
			GPIO_SetBits( GPIOB , GPIO_Pin_14 );
			delay_ms_rough(dly_tim);
			GPIO_ResetBits( GPIOB , GPIO_Pin_14 );
			delay_ms_rough(dly_tim);
		}
	}
	else if(idx == 3)
	{
		if( !stat )
		{
			GPIO_SetBits( GPIOB , GPIO_Pin_11 );
			delay_ms_rough(dly_tim);
			GPIO_ResetBits( GPIOB , GPIO_Pin_11 );
			delay_ms_rough(dly_tim);
		}
		else
		{
			GPIO_SetBits( GPIOB , GPIO_Pin_12 );
			delay_ms_rough(dly_tim);
			GPIO_ResetBits( GPIOB , GPIO_Pin_12 );
			delay_ms_rough(dly_tim);
		}
	}
}
