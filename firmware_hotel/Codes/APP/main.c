// SYSTEM CLOCK EQUAL 8MHZ USING HSI
#include "stdio.h"
#include "stm32f0xx.h"
#include "spi.h"
#include "mfrc522.h"
#include "motor.h"
#include "beep.h"
#include "led.h"

#include "uart1.h"

uint16_t ROOM = 1210;

uint8_t ic_buf[64];
uint16_t IC_TYPE;
uint8_t IC_SN[4];
uint8_t IC_KEY[6] = { 0xab, 0xab, 0xab, 0xab, 0xab, 0xab };


uint32_t sys_tmr_ms = 0;
uint32_t start_line = 0;
uint32_t end_line = 0;

char tmpstr[64];

uint8_t rbuf[64];
uint8_t rbufi = 0;

RTC_TimeTypeDef RTC_TimeStructure;

typedef struct _DateTime
{
	uint16_t Year;
	uint8_t Month;
	uint8_t Day;
	
	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;
}DateTime;

DateTime datetime;

void datetime_Set( uint16_t y, uint8_t M,  uint8_t d,  uint8_t h,  uint8_t m,  uint8_t s )
{
	datetime.Year = y;
	datetime.Month = M;
	datetime.Day = d;
	datetime.Hour = h;
	datetime.Minute = m;
	datetime.Second = s;
}

void datetime_Tick()
{
	uint8_t md = 31;
	if( datetime.Second == 59 )
	{
		datetime.Second = 0;
		if( datetime.Minute == 59 )
		{
			datetime.Minute = 0;
			if( datetime.Hour == 23 )
			{
				datetime.Hour = 0;
				if( datetime.Month == 1 || datetime.Month == 3 || datetime.Month == 5 || datetime.Month == 7 || datetime.Month == 8 || datetime.Month == 10 || datetime.Month == 12 )
				{
					md = 31;
				}
				else if( datetime.Month == 4 || datetime.Month == 6 || datetime.Month == 9 || datetime.Month == 11 )
				{
					md = 30;
				}
				else
				{
					if( ( datetime.Year % 4 == 0 && datetime.Year % 100 != 0 ) || datetime.Year % 400 == 0 )
					{
						md = 29;
					}
					else
					{
						md = 28;
					}
				}
				if( datetime.Day == md )
				{
					datetime.Day = 1;
					if( datetime.Month == 12 )
					{
						datetime.Month = 1;
						datetime.Year ++;
					}
					else
					{
						datetime.Month ++;
					}
				}
				else
				{
					datetime.Day ++;
				}
			}
			else
			{
				datetime.Hour ++;
			}
		}
		else
		{
			datetime.Minute ++;
		}
	}
	else
	{
		datetime.Second ++;
	}
}

void delay_ms_block(int time)
{

   int i=0;
   while(time--) 
   { 
      i=1000;
      while(i--);
   }
}
__IO uint32_t LsiFreq = 0;
__IO uint32_t CaptureNumber = 0, PeriodValue = 0;
uint32_t GetLSIFrequency(void)
{
  NVIC_InitTypeDef   NVIC_InitStructure;
  TIM_ICInitTypeDef  TIM_ICInitStructure;
  RCC_ClocksTypeDef  RCC_ClockFreq;

  /* TIM14 configuration *******************************************************/ 
  /* Enable TIM14 clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
  
  /* Reset TIM14 registers */
  TIM_DeInit(TIM14);

  /* Configure TIM14 prescaler */
  TIM_PrescalerConfig(TIM14, 0, TIM_PSCReloadMode_Immediate);

  /* Connect internally the TIM14_CH1 to the RTC clock output */
  TIM_RemapConfig(TIM14, TIM14_RTC_CLK);

  /* TIM14 configuration: Input Capture mode ---------------------
     The reference clock(LSE or external) is connected to TIM14 CH1
     The Rising edge is used as active edge,
     The TIM14 CCR1 is used to compute the frequency value 
  ------------------------------------------------------------ */
  TIM_ICInitStructure.TIM_Channel     = TIM_Channel_1;
  TIM_ICInitStructure.TIM_ICPolarity  = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV8;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
  TIM_ICInit(TIM14, &TIM_ICInitStructure);

  /* Enable the TIM14 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM14_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable TIM14 counter */
  TIM_Cmd(TIM14, ENABLE);

  /* Reset the flags */
  TIM14->SR = 0;
    
  /* Enable the CC1 Interrupt Request */  
  TIM_ITConfig(TIM14, TIM_IT_CC1, ENABLE);


  /* Wait until the TIM14 get 2 LSI edges (refer to TIM14_IRQHandler() in 
    stm32F0xx_it.c file) ******************************************************/
  while(CaptureNumber != 2)
  {
  }
  /* Deinitialize the TIM14 peripheral registers to their default reset values */
  TIM_DeInit(TIM14);


  /* Compute the LSI frequency, depending on TIM14 input clock frequency (PCLK1)*/
  /* Get SYSCLK, HCLK and PCLKx frequency */
  RCC_GetClocksFreq(&RCC_ClockFreq);
  
  /* PCLK1 prescaler equal to 1 => TIMCLK = PCLK1 */
  return ((RCC_ClockFreq.PCLK_Frequency / PeriodValue) * 8);
}

void Sleep( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIOF, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_Init(GPIOF, &GPIO_InitStructure);	
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// for spi
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

	// for output pin
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// for input pin
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_Init(GPIOB, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	// for rc522
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	GPIO_ResetBits(GPIOF, GPIO_Pin_0);
	GPIO_ResetBits(GPIOF, GPIO_Pin_1);
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_0);
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
//	GPIO_ResetBits(GPIOA, GPIO_Pin_2);
//	GPIO_ResetBits(GPIOA, GPIO_Pin_3);
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
	GPIO_ResetBits(GPIOA, GPIO_Pin_5);
	GPIO_ResetBits(GPIOA, GPIO_Pin_6);
	GPIO_ResetBits(GPIOA, GPIO_Pin_7);

	GPIO_ResetBits(GPIOA, GPIO_Pin_8);
	GPIO_ResetBits(GPIOA, GPIO_Pin_9);
	GPIO_ResetBits(GPIOA, GPIO_Pin_10);
	GPIO_ResetBits(GPIOA, GPIO_Pin_11);
	GPIO_ResetBits(GPIOA, GPIO_Pin_12);
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_15);	
	GPIO_ResetBits(GPIOB, GPIO_Pin_3);
	GPIO_ResetBits(GPIOB, GPIO_Pin_4);
	GPIO_ResetBits(GPIOB, GPIO_Pin_5);
	
	GPIO_ResetBits(GPIOB, GPIO_Pin_6);
	


	//PWR_EnterSTANDBYMode();
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);

}

void RTC_Config(void)
{
//  RTC_TimeTypeDef   RTC_TimeStructure;
  RTC_InitTypeDef   RTC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* RTC Configuration **********************************************************/ 
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  
  /* Allow access to RTC */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset back up registers */
  RCC_BackupResetCmd(ENABLE);
  RCC_BackupResetCmd(DISABLE);
  
  /* Enable the LSE */
  //RCC_LSIConfig(RCC_LSI_ON);
	RCC_LSICmd(ENABLE);
  
  /* Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
  
  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
  
  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);
 
  RTC_DeInit(); 
  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();  
  
	LsiFreq = GetLSIFrequency();
  /* Set RTC calendar clock to 1 HZ (1 second) */
  RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
  RTC_InitStructure.RTC_AsynchPrediv = 99;
  RTC_InitStructure.RTC_SynchPrediv = (LsiFreq/100) - 1;
  
  if (RTC_Init(&RTC_InitStructure) == ERROR)
  {
    while(1);
  }
    
  /* Configure EXTI line 17 (connected to the RTC Alarm event) */
  EXTI_ClearITPendingBit(EXTI_Line17);
  EXTI_InitStructure.EXTI_Line = EXTI_Line17;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* NVIC configuration */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 
	
	//RTC_ITConfig(RTC_IT_TS, ENABLE);
}




void Time_Set( uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec )
{
	RTC_TimeTypeDef   RTC_TimeStructure;
	
	RTC_TimeStructure.RTC_Hours   = hour;
	RTC_TimeStructure.RTC_Minutes = min;
	RTC_TimeStructure.RTC_Seconds = sec;

	RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);	
}

void Alarm_Set()
{
	RTC_TimeTypeDef   RTC_TimeStructure;
  RTC_AlarmTypeDef  RTC_AlarmStructure;
	uint8_t h, m, s;
	
	RTC_ITConfig(RTC_IT_ALRA, DISABLE);
  RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
	
  /* Get current time */
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	
	h = RTC_TimeStructure.RTC_Hours;
	m = RTC_TimeStructure.RTC_Minutes;
	s = RTC_TimeStructure.RTC_Seconds;
	
	s += 1;
	
	if( s >= 60 )
	{
		s -= 60;
		m ++;
	}
	if( m >= 60 )
	{
		m -= 60;
		h ++;
	}
	if( h >= 24 )
	{
		h -= 24;
	}
	
  /* Set the alarm to current time + 5s */
  RTC_AlarmStructure.RTC_AlarmTime.RTC_H12     = RTC_H12_AM;
  RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   = h;
  RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = m;
  RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = s;
  RTC_AlarmStructure.RTC_AlarmDateWeekDay = 31;
  RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
  RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay | RTC_AlarmMask_Minutes |
                                     RTC_AlarmMask_Hours;
  RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
	
	//RTC_AlarmSubSecondConfig(RTC_Alarm_A, 0xff, RTC_AlarmSubSecondMask_SS14_8);
   
  /* Enable the RTC Alarm A interrupt */
  RTC_ITConfig(RTC_IT_ALRA, ENABLE);

  /* Enable the alarm */
  RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
    
  /* Clear the Alarm A Pending Bit */
  //RTC_ClearITPendingBit(RTC_IT_ALRA);  
	
	
}



void EXTI4_15_Config(void)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
  
  /* Configure PC8 and PC13 pins as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
   /* Connect EXTI8 Line to PC8 pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource4);
  
  /* Connect EXTI13 Line to PC13 pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource5);
  
  /* Configure EXTI8 line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line4;  
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* Configure EXTI13 line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line5;
  EXTI_Init(&EXTI_InitStructure);
  
  /* Enable and set EXTI4_15 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}

void door_open()
{
	uint32_t open_timeout=0;
	MOTOR_Enable_Pos();
	delay_ms_block(100);
	MOTOR_Disable();

	GPIO_SetBits(GPIOA, GPIO_Pin_15);
	while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3))
	{
		delay_ms_block(100);
		open_timeout+=100;
		if(open_timeout>=10000)
		{
			break;
		}
	}

	MOTOR_Enable_Neg();
	delay_ms_block(100);
	MOTOR_Disable();
}

int main()
{
	
	//RCC_GetClocksFreq(&rcc_clocks);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	UART1_Init();
	
	SPI1_Init();
	MOTOR_Init();
	BEEP_Init();	
	LED_Init();	
	//EXTI4_15_Config();
	
	datetime_Set( 2017, 2, 13, 22, 15, 30 );
	
	SysTick_Config(8000);
	
	
	RTC_Config();
	//RTC_AlarmConfig();
	//Alarm_Set();
	
//	MOTOR_Enable_Pos();
//	delay_ms_block(100);
//	MOTOR_Disable();

	//PB3小锁钩 高电平=收回 低电平=伸出
	//PB4大锁钩 高电平=伸出 低电平=收回
	
	//换电池不管锁BUG
	MOTOR_Enable_Neg();
	delay_ms_block(100);
	MOTOR_Disable();
	while(1)
	{

//		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5))
//		{
//			BEEP_Tone( 3 );
//		}
//		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3))
//		{
//			LED1_Enable();
//		}
//		else
//		{
//			LED1_Disable();
//		}
//		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4))
//		{
//			LED2_Enable();
//		}
//		else
//		{
//			LED2_Disable();
//		}
//		continue;
		//d=GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5);
		

//						
//		
//		
//		

//		
//		
		
		//continue;
		SPI1_Init();
		MOTOR_Init();
		BEEP_Init();	
		LED_Init();	
		
		MFRC522_Init();
		MFRC522_Enable();
		//MFRC522_PIN_CS(0);
		MFRC522_PIN_RST(1);
		
		Rc522Init();
		
		//if()
		
		if( datetime.Second % 10 == 0 )
		{
			LED1_Enable();
		}
		
		sprintf( tmpstr, "[room] %d\n[time] %d.%d.%d %02d:%02d:%02d\n", ROOM, datetime.Year, datetime.Month, datetime.Day, datetime.Hour, datetime.Minute, datetime.Second );
		UART1_SendString( tmpstr );
		
		if( PcdRequest( PICC_REQALL , ic_buf ) == MI_OK )
		{
			IC_TYPE = ic_buf[ 0 ];
			IC_TYPE <<= 8;
			IC_TYPE |= ic_buf[ 1 ];
			
			if( IC_TYPE == 0x0400 )
			{
				LED1_Enable();
				
				if( PcdAnticoll( ic_buf ) == MI_OK )
				{
					IC_SN[ 0 ] = ic_buf[ 0 ];
					IC_SN[ 1 ] = ic_buf[ 1 ];
					IC_SN[ 2 ] = ic_buf[ 2 ];
					IC_SN[ 3 ] = ic_buf[ 3 ];
					
					
					if( PcdSelect( IC_SN ) == MI_OK )
					{
						if( PcdAuthState( PICC_AUTHENT1A , 4 , IC_KEY , IC_SN ) == MI_OK )
						{
							if( PcdRead( 4, ic_buf ) == MI_OK )
							{
								BEEP_Tone( 4 );
								if( ic_buf[ 0 ] == 1 )
								{
									// admin card
									LED2_Enable();
									BEEP_Tone( 7 );
									BEEP_Tone( 6 );
									BEEP_Tone( 1 );
									
									door_open();
								}
								else if( ic_buf[ 0 ] == 2 )
								{
									uint16_t r;
									r = ic_buf[2];
									// floor card
									LED2_Enable();
									BEEP_Tone( 7 );
									BEEP_Tone( 6 );
									BEEP_Tone( 2 );
									
									if( ROOM / 100 == r )
									{
										door_open();
									}
								}
								else if( ic_buf[ 0 ] == 3 )
								{
									// time card
									uint16_t y = ic_buf[3];
									y <<= 8;
									y |= ic_buf[4];
									
									datetime_Set( y, ic_buf[5], ic_buf[6], ic_buf[7], ic_buf[8], ic_buf[9] );
									LED2_Enable();
									BEEP_Tone( 7 );
									BEEP_Tone( 6 );
									BEEP_Tone( 3 );
								}
								else if( ic_buf[ 0 ] == 4 )
								{
									uint32_t D=0,D2=0;
									uint32_t tmp;
									uint16_t r;
									
									uint16_t y,M,d,H;
									y = ic_buf[3];
									y <<= 8;
									y |= ic_buf[4];
									
									M = ic_buf[5];
									d = ic_buf[6];
									H = ic_buf[7];
									
									r = ic_buf[1];
									r <<= 8;
									r |= ic_buf[2];
									
									tmp=y*1000000;
									D+=tmp;
									tmp=M*10000;
									D+=tmp;
									tmp=d*100;
									D+=tmp;
									D+=H;
									
									tmp=datetime.Year*1000000;
									D2+=tmp;
									tmp=datetime.Month*10000;
									D2+=tmp;
									tmp=datetime.Day*100;
									D2+=tmp;
									D2+=datetime.Hour;
									
									// room card
									LED2_Enable();
									BEEP_Tone( 7 );
									BEEP_Tone( 6 );
									BEEP_Tone( 4 );//2017010116
									
									
									if( r == ROOM )
									{
										if( D >= D2 )
										{
											door_open();
										}
									}
								}
								else if( ic_buf[ 0 ] == 5 )
								{
									// number card
									LED2_Enable();
									BEEP_Tone( 7 );
									BEEP_Tone( 6 );
									BEEP_Tone( 5 );
									
									ROOM=ic_buf[1];
									ROOM<<=8;
									ROOM|=ic_buf[2];
								}
								else
								{													
									BEEP_Tone( 1 );
									BEEP_Tone( 1 );
								}
							}
						}
					}
				}
			}
		}
		//end_line = sys_tmr_ms;
		//delay_ms_block(600);
		
		LED2_Disable();
		LED1_Disable();		
		
		Alarm_Set();		
		
		//PcdHalt();
		//RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, DISABLE);
		//WriteRawRC(CommandReg,0x30);
//		ic_type = ReadRawRC(CommandReg);
		MFRC522_Disable();
		
		//delay_ms_block(1000);
		Sleep();
	}
}

void EXTI4_15_IRQHandler( void )
{
	if(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4))
	{
		LED1_Enable();
		delay_ms_block(50);
		LED1_Disable();
		EXTI_ClearITPendingBit(EXTI_Line4);
	}
	if(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5))
	{
		LED2_Enable();
		delay_ms_block(50);
		LED2_Disable();
		EXTI_ClearITPendingBit(EXTI_Line5);
	}
}

void USART1_IRQHandler( void )
{
	uint8_t data;	

	
	if(USART_GetITStatus(USART1, USART_IT_RXNE))
	{
		data = USART_ReceiveData(USART1);
		rbuf[ rbufi ] = data;
	}
	if(USART_GetITStatus(USART1, USART_IT_IDLE))
	{
		
		USART_ClearITPendingBit(USART1, USART_IT_IDLE);
	}
	
	USART_ClearITPendingBit( USART1, USART_IT_ERR );
	USART_ClearITPendingBit( USART1, USART_IT_ORE );

}
void RTC_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_ALRA) != RESET)
  {
		datetime_Tick();
    RTC_ClearITPendingBit(RTC_IT_ALRA);
    EXTI_ClearITPendingBit(EXTI_Line17);
  } 
}
static uint32_t IC1ReadValue1 = 0, IC1ReadValue2 =0;
void TIM14_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM14, TIM_IT_CC1) != RESET)
  {
    /* Clear TIM14 Capture Compare 1 interrupt pending bit */
    TIM_ClearITPendingBit(TIM14, TIM_IT_CC1);
    
    if(CaptureNumber == 0)
    {
      /* Get the Input Capture value */
      IC1ReadValue1 = TIM_GetCapture1(TIM14);
      CaptureNumber = 1;
    }
    else if(CaptureNumber == 1)
    {
       /* Get the Input Capture value */
       IC1ReadValue2 = TIM_GetCapture1(TIM14); 
       TIM_ITConfig(TIM14, TIM_IT_CC1, DISABLE);

       /* Capture computation */
       if (IC1ReadValue2 > IC1ReadValue1)
       {
         PeriodValue = (IC1ReadValue2 - IC1ReadValue1);
       }
       else
       {
         PeriodValue = ((0xFFFF - IC1ReadValue1) + IC1ReadValue2);
       }
       /* capture of two values is done */
       CaptureNumber = 2;
    }
  }
}
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	sys_tmr_ms++;
}

void SPI1_IRQHandler(void)
{
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
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

