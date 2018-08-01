#ifndef MOTOR_H
#define MOTOR_H

#include "stm32f0xx.h"

void MOTOR_Init( void );
void MOTOR_Disable( void );
void MOTOR_Enable_Pos( void );
void MOTOR_Enable_Neg( void );

#endif
