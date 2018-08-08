
#include "stm32f10x.h"

#include "mew_stm32.h"
#include "mew_m26.h"
#include "mew_board.h"

#include "stdio.h"

int main(void)
{
	mew_board_Init();
	while(1)
	{
		mew_m26.Socket_Schedule_NoOS();
	}
}
