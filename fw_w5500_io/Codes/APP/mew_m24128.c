#include "mew_m24128.h"

#include "mew_iic.h"

#define SELECT_CODE				0xa0

extern void mew_m24128_dl_dly_ms_5_hook(void);

extern void mew_m24128_sda_mode_input_hook(void);
extern void mew_m24128_sda_mode_output_hook(void);

extern void mew_m24128_wc_wr_h_hook(void);
extern void mew_m24128_wc_wr_l_hook(void);

int8_t mew_m24128_wr_byte(uint16_t addr, uint8_t byte)
{	
	mew_m24128_wc_wr_l_hook();
	
	mew_iic_start();
	
	mew_iic_wr_byte(SELECT_CODE);
	if(mew_iic_rd_ack() > 0)
	{
		mew_iic_stop();	
		mew_m24128_wc_wr_h_hook();
		return -1;
	}
	
	mew_iic_wr_byte(addr >> 8);
	if(mew_iic_rd_ack() > 0)
	{
		mew_iic_stop();	
		mew_m24128_wc_wr_h_hook();
		return -1;
	}
	
	mew_iic_wr_byte(addr);
	if(mew_iic_rd_ack() > 0)
	{
		mew_iic_stop();	
		mew_m24128_wc_wr_h_hook();
		return -1;
	}
	
	mew_iic_wr_byte(byte);
	if(mew_iic_rd_ack() > 0)
	{
		mew_iic_stop();	
		mew_m24128_wc_wr_h_hook();
		return -1;
	}
	
	mew_iic_stop();
	
	mew_m24128_wc_wr_h_hook();
	
	// write protect delay
	mew_m24128_dl_dly_ms_5_hook();
	
	return 0;
}

int8_t mew_m24128_rd_byte(uint16_t addr, uint8_t *byte)
{
	mew_iic_start();
	
	mew_iic_wr_byte(SELECT_CODE);
	if(mew_iic_rd_ack() > 0)
	{
		mew_iic_stop();
		return -1;
	}
	
	mew_iic_wr_byte(addr >> 8);
	if(mew_iic_rd_ack() > 0)
	{
		mew_iic_stop();
		return -1;
	}
	mew_iic_wr_byte(addr);
	if(mew_iic_rd_ack() > 0)
	{
		mew_iic_stop();
		return -1;
	}
	
	mew_iic_start();
	
	mew_iic_wr_byte(SELECT_CODE | 1);
	if(mew_iic_rd_ack() > 0)
	{
		mew_iic_stop();
		return -1;
	}
	
	mew_iic_rd_byte(byte);
	
	mew_iic_wr_nack();
	
	mew_iic_stop();
	
	return 0;
}

static int8_t mew_m24128_wr_buff_in_page(uint16_t addr, uint8_t *buff, uint16_t len)
{
	uint16_t i;
	
	if(!len)
		return 0;
	
	mew_m24128_wc_wr_l_hook();
	
	mew_iic_start();
	
	mew_iic_wr_byte(SELECT_CODE);
	if(mew_iic_rd_ack() > 0)
	{
		mew_iic_stop();	
		mew_m24128_wc_wr_h_hook();
		return -1;
	}
	
	mew_iic_wr_byte(addr >> 8);
	if(mew_iic_rd_ack() > 0)
	{
		mew_iic_stop();	
		mew_m24128_wc_wr_h_hook();
		return -1;
	}
	mew_iic_wr_byte(addr);
	if(mew_iic_rd_ack() > 0)
	{
		mew_iic_stop();	
		mew_m24128_wc_wr_h_hook();
		return -1;
	}
	
	for(i = 0; i < len; i++)
	{
		mew_iic_wr_byte(*buff++);
		if(mew_iic_rd_ack() > 0)
		{
			mew_iic_stop();	
			mew_m24128_wc_wr_h_hook();
			return -1;
		}		
	}
	
	mew_iic_stop();	
	mew_m24128_wc_wr_h_hook();
	
	// write protect delay
	mew_m24128_dl_dly_ms_5_hook();
	
	return 0;
}



static int8_t mew_m24128_rd_buff_in_page(uint16_t addr, uint8_t *buff, uint16_t len)
{
	uint16_t i;
	
	if(!len)
		return 0;	
	
	mew_iic_start();
	
	mew_iic_wr_byte(SELECT_CODE);
	if(mew_iic_rd_ack() > 0)
	{
		mew_iic_stop();
		return -1;
	}
	
	mew_iic_wr_byte(addr >> 8);
	if(mew_iic_rd_ack() > 0)
	{
		mew_iic_stop();
		return -1;
	}
	mew_iic_wr_byte(addr);
	if(mew_iic_rd_ack() > 0)
	{	
		mew_iic_stop();
		return -1;
	}
	
	mew_iic_start();
	
	mew_iic_wr_byte(SELECT_CODE | 1);
	if(mew_iic_rd_ack() > 0)
	{	
		mew_iic_stop();		
		return -1;
	}
	
	for(i = 0; i < len; i++)
	{
		mew_iic_rd_byte(buff++);
		
		if(i == len - 1)// the last byte read
			mew_iic_wr_nack();
		else		
			mew_iic_wr_ack();
	}
	
	mew_iic_stop();
	
	return 0;
}

int8_t mew_m24128_wr_buff(uint16_t addr, uint8_t *buff, uint16_t len)
{
	char head_block_cnt;
	char tail_block_cnt;
	char page_cnt;
	char i;
	
	if(addr % 64 == 0)
		head_block_cnt = 0;
	else		
		head_block_cnt = PAGE_SIZE - (addr % PAGE_SIZE);
	
	if(len > head_block_cnt)
	{
		tail_block_cnt = len - head_block_cnt;
		tail_block_cnt %= PAGE_SIZE;
		page_cnt = (len - head_block_cnt) / PAGE_SIZE;
	}
	else
	{
		head_block_cnt = len;
		tail_block_cnt = 0;
		page_cnt = 0;
	}
	
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

int8_t mew_m24128_rd_buff(uint16_t addr, uint8_t *buff, uint16_t len)
{
	char head_block_cnt;
	char tail_block_cnt;
	char page_cnt;
	char i;
	
	if(addr % 64 == 0)
		head_block_cnt = 0;
	else		
		head_block_cnt = PAGE_SIZE - (addr % PAGE_SIZE);
	
	if(len > head_block_cnt)
	{
		tail_block_cnt = len - head_block_cnt;
		tail_block_cnt %= PAGE_SIZE;
		page_cnt = (len - head_block_cnt) / PAGE_SIZE;
	}
	else
	{
		head_block_cnt = len;
		tail_block_cnt = 0;
		page_cnt = 0;
	}
	
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
