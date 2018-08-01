#include "mwbuff.h"

#include "stdlib.h"
#include "string.h"

uint8_t mwBuffInit(mwBuffHandle_t mwBuff, uint16_t size)
{
	mwBuff.pBuff = malloc(size);
	mwBuff.Size = size;
	if(mwBuff.pBuff != NULL)
	{
		mwBuff.Length = 0;
		return 1;
	}
	return 0;
}

uint8_t mwBuffPush(mwBuffHandle_t mwBuff, uint8_t data)
{
	if(mwBuff.Length < mwBuff.Size)
	{
		mwBuff.pBuff[mwBuff.Length ++] = data;
		return 1;
	}
	return 0;
}

int16_t mwBuffSearch(mwBuffHandle_t src, mwBuffHandle_t dst)
{
  uint16_t i = 0;
	if(src.Length < dst.Length)
	{
		return -1;
	}
  for(i = 0; i < src.Length; i++)
  {
    if(src.pBuff[i] == dst.pBuff[0])
    {
      if(memcmp(src.pBuff + i, dst.pBuff, dst.Length) == 0)
      {
        return i;
      }
    }
  }
  return -1;
}
int16_t mwBuffSearchFrom(mwBuffHandle_t src, uint16_t offset, mwBuffHandle_t dst)
{
  uint16_t i = 0;
	if(src.Length < offset + dst.Length)
	{
		return -1;
	}
  for(i = offset; i < src.Length; i++)
  {
    if(src.pBuff[i] == dst.pBuff[0])
    {
      if(memcmp(src.pBuff + i, dst.pBuff, dst.Length) == 0)
      {
        return i;
      }
    }
  }
  return -1;
}
uint8_t mwBuffOffsetWith(uint16_t offset, mwBuffHandle_t src, mwBuffHandle_t dst)
{  
  if(memcmp(src.pBuff + offset, dst.pBuff, dst.Length) == 0)
  {
    return 1;
  }
  
  return 0;
}
uint8_t mwBuffStartWith(mwBuffHandle_t src, mwBuffHandle_t dst)
{  
  if(memcmp(src.pBuff, dst.pBuff, dst.Length) == 0)
  {
    return 1;
  }
  
  return 0;
}

uint8_t mwBuffEndWith(mwBuffHandle_t src, mwBuffHandle_t dst)
{  
  if(memcmp(src.pBuff + (src.Length - dst.Length), dst.pBuff, dst.Length) == 0)
  {
    return 1;
  }
  
  return 0;
}
