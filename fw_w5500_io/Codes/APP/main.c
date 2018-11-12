#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "stm32f10x.h"
#include "freertos.h"
#include "task.h"
#include "cJSON.h"
#include "mew_board.h"
#include "mew_m24128.h"
#include "task_udp.h"
#include "task_relay.h"

//cJSON_Hooks cjson_hooks;

uint8_t dest_ip[4] = {192, 168, 1, 100};
uint16_t dest_port = 9000;

uint8_t dataBuffer[2048]={0};

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
	xTaskCreate(task_led_blink,
	"led_blink", 
	configMINIMAL_STACK_SIZE, //stack depth
	NULL,//parameters
	1,//priority
	NULL);
	
	xTaskCreate(task_udp, 
	"udp", 
	configMINIMAL_STACK_SIZE, 
	NULL, 
	6, 
	NULL );
	
	xTaskCreate(task_relay,	
	"relay", 
	configMINIMAL_STACK_SIZE, 
	NULL , 
	4 , 
	NULL );	
}

uint8_t test_byte;
uint8_t bf[256];
uint8_t br[256];
uint8_t ii;

int main(void)
{
	mew_board_init();
	
	mew_board.led_d5(0);
	mew_board.led_d6(0);

//	mew_board.rly_wr(0, 0);
//	mew_board.rly_wr(1, 0);
//	mew_board.rly_wr(2, 0);
//	mew_board.rly_wr(3, 0);	
	mew_board.rly_rst();
	
	tasks_init();
	
	cJSON_Hooks cjson_hooks;
	cjson_hooks.malloc_fn = pvPortMalloc;
	cjson_hooks.free_fn = vPortFree;
	cJSON_InitHooks(&cjson_hooks);
	
	for(ii = 0; ii < 192; ii++)
	{
		bf[ii] = ii+7;
	}
	
//	if(mew_m24128_wr_buff_in_page(0, bf, 64))
//	{
//		mew_board.led_d5(1);
//	}
//	if(mew_m24128_wr_buff(64, bf+64, 64))
//	{
//		mew_board.led_d5(1);
//	}
//	if(mew_m24128_wr_buff(128, bf+128, 64))
//	{
//		mew_board.led_d5(1);
//	}
	for(ii = 0; ii < 192; ii++)
	{
		bf[ii] = 100;
	}
	if(mew_m24128_wr_buff(0, bf, 128))
	{
		mew_board.led_d5(1);
	}
	for(ii = 0; ii < 192; ii++)
	{
		bf[ii] = ii+0;
	}
	if(mew_m24128_wr_buff(10, bf, 15))
	{
		mew_board.led_d5(1);
	}
	if(mew_m24128_rd_buff(0, br, 192))
	{
		mew_board.led_d5(1);
	}
	
	vTaskStartScheduler();
	
	while(1);
}
