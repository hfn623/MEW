#ifndef MWSTM32_H
#define MWSTM32_H

#include "stdint.h"

typedef struct mwSTM32Handle_t
{
	void (*DelayMS)(uint32_t span);
	void (*PinM26PK)(uint8_t on);
	void (*PinM26NET)(uint8_t on);
	void (*UARTSendByte)(uint8_t port, uint8_t byte);
	void (*UARTSendString)(uint8_t port, char *str);
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

__weak void mwSTM32UARTRecvByteHook(uint8_t port, uint8_t byte);
__weak void mwSTM32UARTRecvDoneHook(uint8_t port);

void mwSTM32Init(void);

#endif
