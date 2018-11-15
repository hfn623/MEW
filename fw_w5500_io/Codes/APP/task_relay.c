#include "task_relay.h"

#include <stdint.h>
#include "freertos.h"
#include "queue.h"
#include "mew_board.h"

QueueHandle_t que_rly;

void task_relay(void *parm)
{
	uint8_t msg;
	que_rly = xQueueCreate(32, 1);
	while(1)
	{
		if(xQueueReceive(que_rly, &msg, portMAX_DELAY ) == pdTRUE)
		{
			mew_board.led_d6(1);
			mew_board.rly_wr(msg & 7, msg >> 7);
			vTaskDelay(10);
			mew_board.rly_rst();
			mew_board.led_d6(0);
			vTaskDelay(10);
		}
	}
}
