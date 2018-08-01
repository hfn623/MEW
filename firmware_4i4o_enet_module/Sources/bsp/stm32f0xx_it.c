/**
  ******************************************************************************
  * @file    Project/STM32F0xx_StdPeriph_Templates/stm32f0xx_it.c 
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    05-December-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_it.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/
void USART1_IRQHandler()
{
	//BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if( USART_GetITStatus( USART1 , USART_IT_RXNE ) )
	{
		uint16_t data = USART_ReceiveData(USART1);
		//BUFF_Push( data );
	}
	if( USART_GetITStatus( USART1 , USART_IT_IDLE ) )
	{
		
		USART_ClearITPendingBit( USART1 , USART_IT_IDLE );
	}
	
	//portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
void USART2_IRQHandler()
{
	//BaseType_t lHigherPriorityTaskWoken = pdFALSE;
	//uint32_t ulPreviousMask;
	
	//ulPreviousMask = portSET_INTERRUPT_MASK_FROM_ISR();
	/* Clear the interrupt if necessary. */
	//Dummy_ClearITPendingBit();
	if( USART_GetITStatus( USART2 , USART_IT_RXNE ) )
	{
		uint16_t data = USART_ReceiveData(USART2);
		BUFF_Push( data );
	}
	if( USART_GetITStatus( USART2 , USART_IT_IDLE ) )
	{
		xSemaphoreGiveFromISR( sem_UART2Rcv, NULL );
		USART_ClearITPendingBit( USART2 , USART_IT_IDLE );
	}
	/* This interrupt does nothing more than demonstrate how to synchronise a
	task with an interrupt.  A semaphore is used for this purpose.  Note
	lHigherPriorityTaskWoken is initialised to zero. Only FreeRTOS API functions
	that end in "FromISR" can be called from an ISR. */
	//xSemaphoreGiveFromISR( semUART2Rcv, &lHigherPriorityTaskWoken );

	/* If there was a task that was blocked on the semaphore, and giving the
	semaphore caused the task to unblock, and the unblocked task has a priority
	higher than the current Running state task (the task that this interrupt
	interrupted), then lHigherPriorityTaskWoken will have been set to pdTRUE
	internally within xSemaphoreGiveFromISR().  Passing pdTRUE into the
	portEND_SWITCHING_ISR() macro will result in a context switch being pended to
	ensure this interrupt returns directly to the unblocked, higher priority,
	task.  Passing pdFALSE into portEND_SWITCHING_ISR() has no effect. */
	//portEND_SWITCHING_ISR( lHigherPriorityTaskWoken );
	
	//portCLEAR_INTERRUPT_MASK_FROM_ISR( ulPreviousMask );
//	uint32_t ulPreviousMask;
//	
//	static signed BaseType_t xHigherPriorityTaskWoken;

//	xHigherPriorityTaskWoken = pdFALSE;
//	
//	ulPreviousMask = portSET_INTERRUPT_MASK_FROM_ISR();
//	
//	
//	
//	

//	xSemaphoreGiveFromISR( semUART2Rcv, &xHigherPriorityTaskWoken );
//	
	//
//	
//	portCLEAR_INTERRUPT_MASK_FROM_ISR( ulPreviousMask );
}
/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
//void SVC_Handler(void)
//{
//}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
//void PendSV_Handler(void)
//{
//}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
//void SysTick_Handler(void)
//{
//}

/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
