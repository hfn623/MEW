#ifndef SPI_H
#define SPI_H

#include "stm32f0xx.h"

void SPI1_Init( void );
uint8_t SPI1_WriteReadByte( uint8_t byte );

#endif
