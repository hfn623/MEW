#include "mew_buff.h"

#include "stdlib.h"
#include "string.h"

uint8_t mew_buff_Init(mew_buff_Handle_t buff, uint16_t size)
{
	buff.pBuff = malloc(size);
	buff.Size = size;
	if(buff.pBuff != NULL)
	{
		buff.Length = 0;
		return 1;
	}
	return 0;
}

uint8_t mew_buff_Push(mew_buff_Handle_t buff, uint8_t data)
{
	if(buff.Length < buff.Size)
	{
		buff.pBuff[buff.Length ++] = data;
		return 1;
	}
	return 0;
}

int16_t mew_buff_Search(mew_buff_Handle_t src, mew_buff_Handle_t dst)
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

int16_t mew_buff_SearchFrom(mew_buff_Handle_t src, uint16_t offset, mew_buff_Handle_t dst)
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

uint8_t mew_buff_OffsetWith(uint16_t offset, mew_buff_Handle_t src, mew_buff_Handle_t dst)
{  
  if(memcmp(src.pBuff + offset, dst.pBuff, dst.Length) == 0)
  {
    return 1;
  }
  
  return 0;
}

uint8_t mew_buff_StartWith(mew_buff_Handle_t src, mew_buff_Handle_t dst)
{  
  if(memcmp(src.pBuff, dst.pBuff, dst.Length) == 0)
  {
    return 1;
  }
  
  return 0;
}

uint8_t mew_buff_EndWith(mew_buff_Handle_t src, mew_buff_Handle_t dst)
{  
  if(memcmp(src.pBuff + (src.Length - dst.Length), dst.pBuff, dst.Length) == 0)
  {
    return 1;
  }
  
  return 0;
}
