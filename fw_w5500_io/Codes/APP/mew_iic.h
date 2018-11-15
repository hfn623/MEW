#ifndef MEW_IIC_H
#define MEW_IIC_H

#include <stdint.h>


void mew_iic_start(void);
void mew_iic_stop(void);
void mew_iic_wr_ack(void);
void mew_iic_wr_nack(void);
uint8_t mew_iic_rd_ack(void);
void mew_iic_wr_byte(uint8_t byte);
void mew_iic_rd_byte(uint8_t *byte);

#endif
