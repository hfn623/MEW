#ifndef MWSTM32_H
#define MWSTM32_H

#include "stdint.h"

typedef struct mwSTM32Handle_t
{
	void (*DelayMS)(uint32_t span);
	void (*PinM26PK)(uint8_t on);
	void (*PinM26NET)(uint8_t on);
	void (*COM1SendByte)(uint8_t byte);
	void (*COM3SendByte)(uint8_t byte);
	void (*DACWrite)(uint16_t val);
	void (*PinLEDTXD)(uint8_t on);
	void (*PinLEDRXD)(uint8_t on);
	void (*PinLEDSTA)(uint8_t on);
	void (*PinBUSY)(uint8_t on);
	void (*PinM26DTR)(uint8_t on);
	uint16_t (*ADCRead)(void);
	uint64_t Nowticks;
	
}mwSTM32Handle_t;

extern mwSTM32Handle_t mwSTM32;

__weak void mwSTM32COM1RecvByteHook(uint8_t byte);
__weak void mwSTM32COM3RecvByteHook(uint8_t byte);

void mwSTM32Init(void);

#endif
