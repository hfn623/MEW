#ifndef BEEP_H
#define BEEP_H

#include "stm32f0xx.h"

void BEEP_Init( void );
//void BEEP_Disable( void );
//void BEEP_Enable( void );
void BEEP_Tone( uint8_t index );

#endif
