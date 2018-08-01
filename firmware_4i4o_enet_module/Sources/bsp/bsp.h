
#include "stm32f0xx.h"

void RCC_Init( void );
void IO_Init( void );



void UART1_Init( void );
void UART1_SendByte( uint8_t byte );
void UART1_SendBuf( uint8_t * pBuff , uint16_t len );
void UART1_SendStr( char * pStr );

void UART2_Init( void );
void UART2_SendByte( uint8_t byte );
void UART2_SendBuf( uint8_t * pBuff , uint16_t len );
void UART2_SendStr( char * pStr );

void delay_ms_rough(uint32_t time);

int8_t OPTOISOLATOR_GET( uint8_t idx );
void LED_SET(uint8_t idx, uint8_t stat);
void RELAY_SET(uint8_t idx, uint8_t stat);
