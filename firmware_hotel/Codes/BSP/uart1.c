
#include "uart1.h"

#include "stm32f0xx.h"

void UART1_Init( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	

	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	// UART1
	GPIO_PinAFConfig( GPIOA, GPIO_PinSource2, GPIO_AF_1 );  
	GPIO_PinAFConfig (GPIOA, GPIO_PinSource3, GPIO_AF_1 );	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init( GPIOA, &GPIO_InitStructure );  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_Parity = USART_Parity_No;  
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_ITConfig( USART1, USART_IT_RXNE, ENABLE );
	USART_ITConfig( USART1, USART_IT_IDLE, ENABLE );
	USART_Cmd( USART1, ENABLE );
}

void UART1_SendByte( uint8_t data )
{
		while( USART_GetFlagStatus( USART1, USART_FLAG_TXE ) == RESET );
		USART_SendData( USART1, data );
}

void UART1_Send( uint8_t *pdata, uint16_t length )
{
	uint16_t i = 0;
	for(i = 0; i < length; i ++)
	{
		UART1_SendByte( *(pdata + i) );
	}
}

void UART1_SendString( char *string )
{
	while( *string)
	{
		UART1_SendByte( *(string ++) );
	}
}
