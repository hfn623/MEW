#include "stm32f10x.h"

#include "mew_board.h"
#include "mew_stm32.h"
#include "mew_mc20.h"

#include "stdio.h"


int main(void)
{
	mew_board_Init();
	
	sprintf(mew_mc20.ADDR[0], "www.boryworks.com");
	mew_mc20.PORT[0] = 9000;
	
	mew_mc20.SocketEnable(0);
	
	mew_board.LED_NS(1);
	mew_board.MC20_PK(0);
	
	while(1)
	{
		mew_mc20.Socket_Schedule_NoOS();
	}
}
