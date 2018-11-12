#include "mew_iic.h"

#include "stm32f10x.h"
#include "mew_board.h"

#define SDA_WR_H()	GPIO_SetBits(GPIOB, GPIO_Pin_7)
#define SDA_WR_L()	GPIO_ResetBits(GPIOB, GPIO_Pin_7)
#define SDA_RD()		GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)?1:0

#define SCL_WR_H()	GPIO_SetBits(GPIOB, GPIO_Pin_6)
#define SCL_WR_L()	GPIO_ResetBits(GPIOB, GPIO_Pin_6)

#define WC_WR_H()		GPIO_SetBits(GPIOB, GPIO_Pin_5)
#define WC_WR_L()		GPIO_ResetBits(GPIOB, GPIO_Pin_5)

#define SELECT_CODE	0xa0


void SDA_MODE_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void SDA_MODE_IN(void)	
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
//	//	GPIO_InitTypeDef GPIO_InitStructure;	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void iic_start(void)
{
	SCL_WR_H();
	SDA_WR_H();	
	SDA_WR_L();
}

void iic_stop(void)
{
	SCL_WR_H();
	SDA_WR_L();	
	SDA_WR_H();
}

void iic_wr_ack(void)
{	
	SCL_WR_L();
	SDA_WR_L();
  SCL_WR_H();
	SCL_WR_L();// necessary for page read
}

void iic_wr_nack(void)
{	
	SCL_WR_L(); 
	SDA_WR_H();
  SCL_WR_H();
	SCL_WR_L();
}

uint8_t iic_rd_ack(void)
{
	uint8_t ack = 0;
	
	SCL_WR_L();
  SCL_WR_H();
	SDA_MODE_IN();
  ack = SDA_RD();
	SDA_MODE_OUT();
  SCL_WR_L();// this is necessary
	
  return ack;
}

void iic_wr_byte(uint8_t byte)
{
	char i;
  for(i = 0; i < 8; i++)
  {
    SCL_WR_L();
    if(byte & 0x80)
    {
      SDA_WR_H();
    }
    else
    {
      SDA_WR_L();
    }
    byte <<= 1;
    SCL_WR_H();
  }
	SCL_WR_L();//important
	//SDA_WR_H(); must delete for write
}

uint8_t iic_rd_byte(void)
{
  char i;
	uint8_t byte = 0;

  for(i = 0; i < 8; i++)
  {
    SCL_WR_L();
    SCL_WR_H();
    byte <<= 1;
    byte |= SDA_RD();
  }
	SCL_WR_L();//important
  return byte;
}

int8_t mew_m24128_wr_byte(uint16_t addr, uint8_t byte)
{	
	WC_WR_L();
	
	iic_start();
	
	iic_wr_byte(SELECT_CODE);
	if(iic_rd_ack() > 0)
	{
		iic_stop();	
		WC_WR_H();
		return -1;
	}
	
	iic_wr_byte(addr >> 8);
	if(iic_rd_ack() > 0)
	{
		iic_stop();	
		WC_WR_H();
		return -1;
	}
	
	iic_wr_byte(addr);
	if(iic_rd_ack() > 0)
	{
		iic_stop();	
		WC_WR_H();
		return -1;
	}
	
	iic_wr_byte(byte);
	if(iic_rd_ack() > 0)
	{
		iic_stop();	
		WC_WR_H();
		return -1;
	}
	
	iic_stop();
	
	WC_WR_H();
	
	// write op take 5ms max
	mew_board.dl_dly_ms(5);
	
	return 0;
}

int8_t mew_m24128_wr_buff_in_page(uint16_t addr, uint8_t* buff, uint16_t len)
{
	uint16_t i;
	
	if(!len)
		return 0;
	
	WC_WR_L();
	
	iic_start();
	
	iic_wr_byte(SELECT_CODE);
	if(iic_rd_ack() > 0)
	{
		iic_stop();	
		WC_WR_H();
		return -1;
	}
	
	iic_wr_byte(addr >> 8);
	if(iic_rd_ack() > 0)
	{
		iic_stop();	
		WC_WR_H();
		return -1;
	}
	iic_wr_byte(addr);
	if(iic_rd_ack() > 0)
	{
		iic_stop();	
		WC_WR_H();
		return -1;
	}
	
	for(i = 0; i < len; i++)
	{
		iic_wr_byte(*buff++);
		if(iic_rd_ack() > 0)
		{
			iic_stop();	
			WC_WR_H();
			return -1;
		}		
	}
	
	iic_stop();	
	WC_WR_H();
	
	// write op take 5ms max
	mew_board.dl_dly_ms(5);
	
	return 0;
}

int8_t mew_m24128_rd_byte(uint16_t addr, uint8_t* byte)
{
	iic_start();
	
	iic_wr_byte(SELECT_CODE);
	if(iic_rd_ack() > 0)
	{
		iic_stop();
		return -1;
	}
	
	iic_wr_byte(addr >> 8);
	if(iic_rd_ack() > 0)
	{
		iic_stop();
		return -1;
	}
	iic_wr_byte(addr);
	if(iic_rd_ack() > 0)
	{
		iic_stop();
		return -1;
	}
	
	iic_start();
	
	iic_wr_byte(SELECT_CODE | 1);
	if(iic_rd_ack() > 0)
	{
		iic_stop();
		return -1;
	}
	
	*byte = iic_rd_byte();
	
	iic_wr_nack();
	
	iic_stop();
	
	return 0;
}

static int8_t mew_m24128_rd_buff_in_page(uint16_t addr, uint8_t* buff, uint16_t len)
{
	uint16_t i;
	
	if(!len)
		return 0;	
	
	iic_start();
	
	iic_wr_byte(SELECT_CODE);
	if(iic_rd_ack() > 0)
	{
		iic_stop();
		return -1;
	}
	
	iic_wr_byte(addr >> 8);
	if(iic_rd_ack() > 0)
	{
		iic_stop();
		return -1;
	}
	iic_wr_byte(addr);
	if(iic_rd_ack() > 0)
	{	
		iic_stop();
		return -1;
	}
	
	iic_start();
	
	iic_wr_byte(SELECT_CODE | 1);
	if(iic_rd_ack() > 0)
	{	
		iic_stop();		
		return -1;
	}
	
	for(i = 0; i < len; i++)
	{		
		
		SDA_MODE_IN();
		*buff++ = iic_rd_byte();
		SDA_MODE_OUT();
		
		if(i == len - 1)// the last byte read
			iic_wr_nack();
		else		
			iic_wr_ack();
	}
	
	iic_stop();
	
	return 0;
}

int8_t mew_m24128_wr_buff(uint16_t addr, uint8_t* buff, uint16_t len)
{
	char head_block_cnt;
	char tail_block_cnt;
	char page_cnt;
	char i;
	
	if(addr % 64 == 0)
		head_block_cnt = 0;
	else		
		head_block_cnt = PAGE_SIZE - (addr % PAGE_SIZE);
	
	tail_block_cnt = len - head_block_cnt;
	tail_block_cnt %= PAGE_SIZE;
	
	page_cnt = (len - head_block_cnt - tail_block_cnt) / PAGE_SIZE;
	
	if(mew_m24128_wr_buff_in_page(addr, buff, head_block_cnt))
	{
		return -1;
	}
	addr += head_block_cnt;
	buff += head_block_cnt;
	
	for(i = 0; i < page_cnt; i++)
	{
		if(mew_m24128_wr_buff_in_page(addr, buff, PAGE_SIZE))
		{
			return -1;
		}
		addr += PAGE_SIZE;
		buff += PAGE_SIZE;
	}
	
	if(mew_m24128_wr_buff_in_page(addr, buff, tail_block_cnt))
	{
		return -1;
	}
	
	return 0;
}

int8_t mew_m24128_rd_buff(uint16_t addr, uint8_t* buff, uint16_t len)
{
	char head_block_cnt;
	char tail_block_cnt;
	char page_cnt;
	char i;
	
	if(addr % 64 == 0)
		head_block_cnt = 0;
	else		
		head_block_cnt = PAGE_SIZE - (addr % PAGE_SIZE);
	
	tail_block_cnt = len - head_block_cnt;
	tail_block_cnt %= PAGE_SIZE;
	
	page_cnt = (len - head_block_cnt - tail_block_cnt) / PAGE_SIZE;
	
	if(mew_m24128_rd_buff_in_page(addr, buff, head_block_cnt))
	{
		return -1;
	}
	addr += head_block_cnt;
	buff += head_block_cnt;
	
	for(i = 0; i < page_cnt; i++)
	{
		if(mew_m24128_rd_buff_in_page(addr, buff, PAGE_SIZE))
		{
			return -1;
		}
		addr += PAGE_SIZE;
		buff += PAGE_SIZE;
	}
	
	if(mew_m24128_rd_buff_in_page(addr, buff, tail_block_cnt))
	{
		return -1;
	}
	
	return 0;
}
