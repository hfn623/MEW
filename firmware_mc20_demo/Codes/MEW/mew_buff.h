#ifndef mew_BUFF_H
#define mew_BUFF_H
#include "stdint.h"

#pragma pack(1)
typedef struct mew_buff_Handle_t
{
	uint8_t *pBuff;
  uint16_t Length;  
	uint16_t *pLength;
	uint16_t Size;
	
}mew_buff_Handle_t;

int16_t mew_buff_Search(mew_buff_Handle_t src, mew_buff_Handle_t dst);
uint8_t mew_buff_StartWith(mew_buff_Handle_t src, mew_buff_Handle_t dst);
uint8_t mew_buff_OffsetWith(uint16_t offset, mew_buff_Handle_t src, mew_buff_Handle_t dst);
uint8_t mew_buff_EndWith(mew_buff_Handle_t src, mew_buff_Handle_t dst);
int16_t mew_buff_SearchFrom(mew_buff_Handle_t src, uint16_t offset, mew_buff_Handle_t dst);

#endif
