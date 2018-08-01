#include "beep.h"

static void delay_ns(uint32_t ns)
{
  uint32_t i;
  for(i=0;i<ns;i++)
  {
    __nop();
    __nop();
    __nop();
  }
}
void BEEP_Init( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;


  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

//void BEEP_Disable( void )
//{
//	GPIO_ResetBits( GPIOA , GPIO_Pin_10 );
//}

//void BEEP_Enable( void )
//{
//	GPIO_SetBits( GPIOA , GPIO_Pin_10 );
//}

void BEEP_Tone( uint8_t index )
{
	uint32_t i = 0;

	if( index < 1 || index > 7 )
	{
		index = 4 ;
	}
	index = 9 - index;
	for( i = 0; i < 2000 / index; i ++ )
	{
		GPIO_SetBits( GPIOA , GPIO_Pin_10 );
		delay_ns( 25 * index );
		GPIO_ResetBits( GPIOA , GPIO_Pin_10 );
		delay_ns( 25 * index );
	}
}
