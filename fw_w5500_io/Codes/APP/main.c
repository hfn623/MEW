#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos.h"
#include "task.h"

#include "mew_board.h"
#include "mew_m24128.h"
#include "task_udp.h"
#include "task_relay.h"
#include "task_http.h"


static void task_led_blink(void *parm)
{
	TickType_t last_wake_time;
	while(1)
	{
		mew_board.led_d5(1);
		vTaskDelayUntil(&last_wake_time, 10);
		mew_board.led_d5(0);
		vTaskDelayUntil(&last_wake_time, 990);			
	}
}

void tasks_init(void)
{
	xTaskCreate(task_udp, 
	"udp", 
	configMINIMAL_STACK_SIZE, 
	NULL, 
	9, 
	NULL );
	

	xTaskCreate(task_http, 
	"http", 
	configMINIMAL_STACK_SIZE, 
	NULL, 
	8, 
	NULL );
	
	xTaskCreate(task_relay,	
	"relay", 
	configMINIMAL_STACK_SIZE, 
	NULL , 
	4 , 
	NULL );	
	
	xTaskCreate(task_led_blink,
	"led_blink", 
	configMINIMAL_STACK_SIZE, //stack depth
	NULL,//parameters
	1,//priority
	NULL);
}

int main(void)
{
	mew_board_init();	
	tasks_init();	
	vTaskStartScheduler();	
	while(1);
}
