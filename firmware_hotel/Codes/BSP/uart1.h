#ifndef UART1_H
#define UART1_H

#include "stdint.h"

void UART1_Init( void );

void UART1_SendByte( uint8_t data );

void UART1_Send( uint8_t *pdata, uint16_t length );

void UART1_SendString( char *string );


#endif
