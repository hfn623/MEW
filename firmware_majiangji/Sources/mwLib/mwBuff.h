#ifndef MWBUFF_H
#define MWBUFF_H
#include "stdint.h"

#pragma pack(1)
typedef struct mwBuffHandle_t
{
	uint8_t *pBuff;
  uint16_t Length;  
	uint16_t *pLength;
	uint16_t Size;
	
}mwBuffHandle_t;

int16_t mwBuffSearch(mwBuffHandle_t src, mwBuffHandle_t dst);
uint8_t mwBuffStartWith(mwBuffHandle_t src, mwBuffHandle_t dst);
uint8_t mwBuffOffsetWith(uint16_t offset, mwBuffHandle_t src, mwBuffHandle_t dst);
uint8_t mwBuffEndWith(mwBuffHandle_t src, mwBuffHandle_t dst);
int16_t mwBuffSearchFrom(mwBuffHandle_t src, uint16_t offset, mwBuffHandle_t dst);

#endif
