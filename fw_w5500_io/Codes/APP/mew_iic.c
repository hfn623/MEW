#include "mew_iic.h"

#include "stm32f10x.h"
#include "mew_board.h"

#define SDA_MODE_OUT			mew_iic_sda_mode_output_hook
#define SDA_MODE_IN				mew_iic_sda_mode_input_hook

#define SDA_WR_H					mew_iic_sda_wr_h_hook
#define SDA_WR_L					mew_iic_sda_wr_l_hook

#define SDA_RD						mew_iic_sda_rd_hook

#define SCL_WR_H					mew_iic_scl_wr_h_hook
#define SCL_WR_L					mew_iic_scl_wr_l_hook


extern void mew_iic_sda_mode_output_hook(void);
extern void mew_iic_sda_mode_input_hook(void);

extern void mew_iic_scl_wr_h_hook(void);
extern void mew_iic_scl_wr_l_hook(void);

extern void mew_iic_sda_wr_h_hook(void);
extern void mew_iic_sda_wr_l_hook(void);

extern uint8_t mew_iic_sda_rd_hook(void);

void mew_iic_start(void)
{
	SDA_MODE_OUT();
	
	SCL_WR_H();
	SDA_WR_H();	
	SDA_WR_L();
}

void mew_iic_stop(void)
{	
	SDA_WR_L();	
	SCL_WR_H();// must stay here!!!
	SDA_WR_H();
}

void mew_iic_wr_ack(void)
{
	SDA_MODE_OUT();
	
	SCL_WR_L();
	SDA_WR_L();
  SCL_WR_H();
	SCL_WR_L();// necessary for page read
}

void mew_iic_wr_nack(void)
{
	SDA_MODE_OUT();
	
	SCL_WR_L(); 
	SDA_WR_H();
  SCL_WR_H();
	SCL_WR_L();
}

uint8_t mew_iic_rd_ack(void)
{
	uint8_t ack = 0;
	SDA_MODE_IN();
	SCL_WR_L();
  SCL_WR_H();	
  ack = SDA_RD();	
  SCL_WR_L();// this is necessary
	SDA_MODE_OUT();
  return ack;
}

void mew_iic_wr_byte(uint8_t byte)
{
	char i;
	SDA_MODE_OUT();
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

void mew_iic_rd_byte(uint8_t *byte)
{
  char i;
	SDA_MODE_IN();
  for(i = 0; i < 8; i++)
  {
    SCL_WR_L();
    SCL_WR_H();
    *byte <<= 1;
    *byte |= SDA_RD();
  }
	SCL_WR_L();//important
	SDA_MODE_OUT();
}

