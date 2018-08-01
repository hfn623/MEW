#ifndef LCD19264_H
#define LCD19264_H

#include "stdint.h"
#include "stm32f10x.h"
//#include "board.h"

extern const uint8_t welcome[];
extern const uint8_t err[];
extern const uint8_t warn[];



void LCD_Power(uint8_t on);
void LCD_LED_Toggle(uint8_t set);
void LCD_Clear(uint8_t c);
void LCD_Write(uint8_t x, uint8_t y, uint8_t d);

void LCD_SetXY(uint8_t x, uint8_t y);

void LCD_DrawPoint(uint8_t x, uint8_t y, uint8_t c);
void LCD_DRAW_FRAME(void);
void LCD_ShowHZ12(uint8_t x, uint8_t y, char *hz);
void LCD_ShowHZ12String(uint8_t x, uint8_t y, char *hz);
void LCD_ShowPIC(const uint8_t *pic);

void LCD_RST_Toggle(uint8_t set);


#endif
