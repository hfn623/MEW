#ifndef MEW_M24128_H
#define MEW_M24128_H

#include <stdint.h>

#define PAGE_SIZE						64

int8_t mew_m24128_wr_byte(uint16_t addr, uint8_t byte);
int8_t mew_m24128_rd_byte(uint16_t addr, uint8_t* byte);

int8_t mew_m24128_wr_buff(uint16_t addr, uint8_t* buff, uint16_t len);
int8_t mew_m24128_rd_buff(uint16_t addr, uint8_t* buff, uint16_t len);

#endif
