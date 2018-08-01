#ifndef __APP_H
#define __APP_H

#include "bsp.h"
#include "buff.h"
#include "freertos.h"
#include "task.h"
#include "semphr.h"

extern SemaphoreHandle_t sem_UART2Rcv; 


void APP_Init( void );


#endif
