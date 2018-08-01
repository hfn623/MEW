#ifndef mew_STM32_H
#define mew_STM32_H

#include "stdint.h"

typedef struct mew_stm32_Handle_t
{
	uint64_t Nowticks;	
	void (*PortAWrite)(uint8_t pin, uint8_t state);
	void (*PortBWrite)(uint8_t pin, uint8_t state);
	void (*PortCWrite)(uint8_t pin, uint8_t state);	
	uint8_t (*PortARead)(uint8_t pin);
	uint8_t (*PortBRead)(uint8_t pin);
	uint8_t (*PortCRead)(uint8_t pin);
	void (*DelayMS)(uint32_t span);
	void (*UARTSendByte)(uint8_t port, uint8_t byte);
	void (*UARTSendString)(uint8_t port, char *str);
	void (*DACWrite)(uint16_t val);
	uint16_t (*ADCRead)(void);
	
}mew_stm32_Handle_t;

extern mew_stm32_Handle_t mew_stm32;

void mew_stm32_Init(uint32_t uart1speed, uint32_t uart2speed, uint32_t uart3speed);

__weak void mew_stm32_RCCInit_Hook(void);
__weak void mew_stm32_PINsInit_Hook(void);

__weak void mew_stm32_UARTRecvByte_Hook(uint8_t port, uint8_t byte);
__weak void mew_stm32_UARTRecvDone_Hook(uint8_t port);
__weak void mew_stm32_SysTick_Hook(void);

#endif
