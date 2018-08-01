#ifndef MWBOARD_H
#define MWBOARD_H

#include "stdint.h"
#include "stm32f10x.h"


void pressKey1(void);
void pressKey2(void);
void pressKey3(void);
void pressKey4(void);

extern uint8_t SW1, SW2, SW3, SW4;
extern uint8_t running;
extern uint8_t keyEnable;

typedef struct mwBoard_t
{
	void (*DelayMS)(uint32_t span);
	void (*COM1SendByte)(uint8_t byte);
	void (*COM2SendByte)(uint8_t byte);
	void (*COM1SendBuff)(uint8_t *buff, uint16_t len);
	void (*COM2SendBuff)(uint8_t *buff, uint16_t len);
	
	uint16_t (*ADCRead)(uint8_t ch);
	uint64_t Nowticks;
	
}mwBoard_t;

extern mwBoard_t mwBoard;

void boardInit(void);

#endif
