#include "spi.h"

void SPI1_Init( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
//  NVIC_InitTypeDef NVIC_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;  
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_0);//sck
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_0);//miso
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_0);//mosi
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;


  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* SPI configuration -------------------------------------------------------*/
  SPI_I2S_DeInit(SPI1);
	
	SPI_StructInit( & SPI_InitStructure );
	
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
	
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_Init(SPI1, &SPI_InitStructure);
   
	SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);
	
	SPI_Cmd(SPI1, ENABLE);
}

uint8_t SPI1_WriteReadByte( uint8_t byte )
{
	while( SPI_I2S_GetFlagStatus( SPI1 , SPI_I2S_FLAG_TXE ) == RESET );
	SPI_SendData8( SPI1 , byte );
	while( SPI_I2S_GetFlagStatus( SPI1 , SPI_I2S_FLAG_RXNE ) == RESET );
	return SPI_ReceiveData8( SPI1 );
}
