#include "led.h"

void LED_Init( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;


  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void LED1_Disable( void )
{
	GPIO_ResetBits( GPIOA , GPIO_Pin_9 );
}

void LED1_Enable( void )
{
	GPIO_SetBits( GPIOA , GPIO_Pin_9 );
}

void LED2_Disable( void )
{
	GPIO_ResetBits( GPIOA , GPIO_Pin_8 );
}

void LED2_Enable( void )
{
	GPIO_SetBits( GPIOA , GPIO_Pin_8 );
}
