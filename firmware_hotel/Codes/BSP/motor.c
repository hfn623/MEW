#include "motor.h"

void MOTOR_Init( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;


  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;



	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_Init(GPIOB, &GPIO_InitStructure);  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	

	//GPIO_SetBits( GPIOA , GPIO_Pin_15 );
}

void MOTOR_Disable( void )
{
	GPIO_ResetBits( GPIOA , GPIO_Pin_11 );
	GPIO_ResetBits( GPIOA , GPIO_Pin_12 );
}

void MOTOR_Enable_Pos( void )
{
	GPIO_ResetBits( GPIOA , GPIO_Pin_11 );
	GPIO_SetBits( GPIOA , GPIO_Pin_12 );
}

void MOTOR_Enable_Neg( void )
{
	GPIO_ResetBits( GPIOA , GPIO_Pin_12 );
	GPIO_SetBits( GPIOA , GPIO_Pin_11 );
}
