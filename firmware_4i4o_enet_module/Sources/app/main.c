
#include "app.h"




int main( void )
{
	delay_ms_rough(100);
	
	BUFF_Init();	
	
	RCC_Init();
	IO_Init();
	
	UART1_Init();
	UART2_Init();
	
	APP_Init();
	
//	RELAY_SET(0, 0);
//	RELAY_SET(1, 0);
//	RELAY_SET(2, 0);
//	RELAY_SET(3, 0);
	
	vTaskStartScheduler();		
	
	for( ; ; );
}
