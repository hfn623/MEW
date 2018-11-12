#ifndef MEW_BOARD_H
#define MEW_BOARD_H

#include <stdint.h>
#include "freertos.h"
#include "semphr.h"
#include "queue.h"

#define BOARD_NAME "ENET_IO_W5500\0"

#define SOCK_OF_UDP 1

typedef struct mew_board_handle_t
{
	void (*led_d5)(uint8_t state);
	void (*led_d6)(uint8_t state);
	uint8_t (*op_rd)(uint8_t port);
	void (*rly_wr)(uint8_t port, uint8_t state);
	void (*rly_rst)(void);
	void (*dl_dly_us)(uint32_t ts);
	void (*dl_dly_ms)(uint32_t ts);
}mew_board_handle_t;

typedef struct net_parms_handle_t
{
	uint8_t MAC[6];
	uint8_t IP[4];	
	uint8_t MASK[4];
	uint8_t GW[4];
	uint8_t DNS[4];
}net_parms_handle_t;

void mew_board_init(void);

extern mew_board_handle_t mew_board;
extern net_parms_handle_t net_parms;
extern SemaphoreHandle_t sem_udp_recv;
extern QueueHandle_t que_udp;
extern QueueHandle_t que_rly;

#define W5500_IRQ_HANDLER EXTI0_IRQHandler

#endif
