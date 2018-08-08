#ifndef mew_BOARD_H
#define mew_BOARD_H

#include "stdint.h"

typedef struct mew_board_Handle_t
{
	void (*LED_NS)(uint8_t state);
	void (*LED_STA)(uint8_t state);
	void (*LED_TX)(uint8_t state);
	void (*LED_RX)(uint8_t state);
	void (*M26_PK)(uint8_t state);
	void (*PIN_WKUP)(uint8_t state);
	void (*PIN_IO)(uint8_t state);
}mew_board_Handle_t;

void mew_board_Init(void);

extern mew_board_Handle_t mew_board;

#endif
