#include "app.h"

static uint8_t relay_reg[ 8 ] = { 0 };
static uint8_t send_buf[128] = { 0 };

static uint16_t mbap_id = 0;
static uint16_t mbap_tag = 0;
static uint16_t mbap_len = 0;
static uint8_t  mbap_addr = 0;

static uint8_t  modbus_fc = 0;

static uint16_t reg_addr = 0;
static uint16_t reg_cnt = 0;
	
SemaphoreHandle_t sem_UART2Rcv; 

static void task_UART2Recv( void * pvParameters )
{
	uint16_t i = 0;
	uint16_t j = 0;
	uint8_t r[8];
	
	for( ; ; )
	{
		if( xSemaphoreTake( sem_UART2Rcv, portMAX_DELAY ) == pdTRUE )
		{
			LED_SET( 1, 1 );
			
			if( BUFF_GetCount() >= 6 )
			{
				mbap_id = BUFF_GetData( 0 );
				mbap_id <<= 8;
				mbap_id |= BUFF_GetData( 1 );
				
				mbap_tag = BUFF_GetData( 2 );
				mbap_tag <<= 8;
				mbap_tag |= BUFF_GetData( 3 );

				if( mbap_tag == 0 )
				{
					//UART2_SendStr( "rcv MBAP header\r\n" );
					
					mbap_len = BUFF_GetData( 4 );
					mbap_len <<= 8;
					mbap_len |= BUFF_GetData( 5 );
					
					if( BUFF_GetCount() >= 6 + mbap_len && mbap_len <= 8 )
					{
						mbap_addr = BUFF_GetData( 6 );
						
						//UART2_SendStr( "rcv MBAP + MODBUS frame\r\n" );
						
						modbus_fc = BUFF_GetData( 7 );
						
						if( modbus_fc == 0x05 )
						{
							reg_addr = BUFF_GetData( 8 );
							reg_addr <<= 8;
							reg_addr |= BUFF_GetData( 9 );
							
							reg_cnt = BUFF_GetData( 10 );
							reg_cnt <<= 8;
							reg_cnt |= BUFF_GetData( 11 );
							
							reg_addr %= 4;
							
//							if(reg_addr >= 0 && reg_addr <=3)
							{							
								r[ reg_addr ] = reg_cnt ? 1 : 0;
								
								if(r[ reg_addr ] != relay_reg[ reg_addr ])
								{
									RELAY_SET( reg_addr, r[ reg_addr ] );
								}
								relay_reg[ reg_addr ] = r[ reg_addr ];
							}
							
							mbap_len = 6;
							
							i = 0;
							send_buf[ i ++ ] = mbap_id >> 8;
							send_buf[ i ++ ] = mbap_id;
							
							send_buf[ i ++ ] = mbap_tag >> 8;
							send_buf[ i ++ ] = mbap_tag;
							
							send_buf[ i ++ ] = mbap_len >> 8;
							send_buf[ i ++ ] = mbap_len;
							
							send_buf[ i ++ ] = mbap_addr;
							
							send_buf[ i ++ ] = modbus_fc;
							
							send_buf[ i ++ ] = reg_addr >> 8;
							send_buf[ i ++ ] = reg_addr;
							
							send_buf[ i ++ ] = reg_cnt >> 8;
							send_buf[ i ++ ] = reg_cnt;
							
							UART2_SendBuf( send_buf , i );
							
							//UART2_SendStr( "rcv function code 0x0f\r\n" );
						}
						else if( modbus_fc == 0x0f )
						{
							reg_addr = BUFF_GetData( 8 );
							reg_addr <<= 8;
							reg_addr |= BUFF_GetData( 9 );
							
							reg_cnt = BUFF_GetData( 10 );
							reg_cnt <<= 8;
							reg_cnt |= BUFF_GetData( 11 );
							
							reg_addr = reg_addr > 3 ? 3 : reg_addr;
							reg_cnt = reg_cnt > 4 ? 4 : reg_cnt;
							
							for( i = reg_addr , j = 0 ; i < reg_addr + reg_cnt ; i ++ , j ++ )
							{
								r[ i ] = ( ( BUFF_GetData( 13 + i / 8 ) & ( 1 << j ) ) > 0 ) ? 1 : 0;
								if(r[ i ] != relay_reg[ i ])
								{
									RELAY_SET(i, r[ i ]);
								}
								relay_reg[ i ] = r[ i ];
							}
							
							mbap_len = 6;
							
							i = 0;
							send_buf[ i ++ ] = mbap_id >> 8;
							send_buf[ i ++ ] = mbap_id;
							
							send_buf[ i ++ ] = mbap_tag >> 8;
							send_buf[ i ++ ] = mbap_tag;
							
							send_buf[ i ++ ] = mbap_len >> 8;
							send_buf[ i ++ ] = mbap_len;
							
							send_buf[ i ++ ] = mbap_addr;
							
							send_buf[ i ++ ] = modbus_fc;
							
							send_buf[ i ++ ] = reg_addr >> 8;
							send_buf[ i ++ ] = reg_addr;
							
							send_buf[ i ++ ] = reg_cnt >> 8;
							send_buf[ i ++ ] = reg_cnt;
							
							UART2_SendBuf( send_buf , i );
							
							//UART2_SendStr( "rcv function code 0x0f\r\n" );
						}
						else if( modbus_fc == 0x01 )
						{
							reg_addr = BUFF_GetData( 8 );
							reg_addr <<= 8;
							reg_addr |= BUFF_GetData( 9 );
							
							reg_cnt = BUFF_GetData( 10 );
							reg_cnt <<= 8;
							reg_cnt |= BUFF_GetData( 11 );
							
							mbap_len = 4;
							
							i = 0;
							send_buf[ i ++ ] = mbap_id >> 8;
							send_buf[ i ++ ] = mbap_id;
							
							send_buf[ i ++ ] = mbap_tag >> 8;
							send_buf[ i ++ ] = mbap_tag;
							
							send_buf[ i ++ ] = mbap_len >> 8;
							send_buf[ i ++ ] = mbap_len;
							
							send_buf[ i ++ ] = mbap_addr;
							
							send_buf[ i ++ ] = modbus_fc;
							
							send_buf[ i ++ ] = 1;
							
							send_buf[ i ] = 0;
							for( j = 0 ; j < 4 ; j ++ )
							{
								send_buf[ i ] |= relay_reg[ j ] ? ( 1 << j ) : 0 ;
							}
							
							UART2_SendBuf( send_buf , ++ i );
						}
						else if( modbus_fc == 0x02 )
						{
							reg_addr = BUFF_GetData( 8 );
							reg_addr <<= 8;
							reg_addr |= BUFF_GetData( 9 );
							
							reg_cnt = BUFF_GetData( 10 );
							reg_cnt <<= 8;
							reg_cnt |= BUFF_GetData( 11 );
							
							mbap_len = 4;
							
							i = 0;
							send_buf[ i ++ ] = mbap_id >> 8;
							send_buf[ i ++ ] = mbap_id;
							
							send_buf[ i ++ ] = mbap_tag >> 8;
							send_buf[ i ++ ] = mbap_tag;
							
							send_buf[ i ++ ] = mbap_len >> 8;
							send_buf[ i ++ ] = mbap_len;
							
							send_buf[ i ++ ] = mbap_addr;
							
							send_buf[ i ++ ] = modbus_fc;
							
							send_buf[ i ++ ] = 1;
							
							send_buf[ i ] = 0;
							send_buf[ i ] |= OPTOISOLATOR_GET(0) ? 1 : 0 ;
							send_buf[ i ] |= OPTOISOLATOR_GET(1) ? 2 : 0 ;
							send_buf[ i ] |= OPTOISOLATOR_GET(2) ? 4 : 0 ;
							send_buf[ i ] |= OPTOISOLATOR_GET(3) ? 8 : 0 ;
							
							UART2_SendBuf( send_buf , ++ i );
						}
						
						BUFF_Remove( BUFF_GetCount() );
						//UART2_SendStr( "rcv buff clear\r\n" );
					}
				}
			}
			LED_SET( 1, 0 );
		}
	}
}

//static void task_RefreshRelay( void * pvParameters )
//{
//	//uint16_t i = 0;
//	for( ; ; )
//	{
//		vTaskDelay( 100 );

//	}
//}

//static void task_Test( void * pvParameters )
//{
//	for( ; ; )
//	{
//		RELAY1( OPTOISOLATOR1() );
//		RELAY2( OPTOISOLATOR2() );
//		RELAY3( OPTOISOLATOR3() );
//		RELAY4( OPTOISOLATOR4() );
//		vTaskDelay( 100 );
//	}
//}

static void task_Heartbeat( void * pvParameters )
{
	for( ; ; )
	{
		LED_SET( 0, 1 );
		vTaskDelay( 30 );
		LED_SET( 0, 0 );
		vTaskDelay( 2970 );			
	}
}


void APP_Init( void )
{
	sem_UART2Rcv = xSemaphoreCreateBinary();
	
	xTaskCreate(task_Heartbeat,
		"task_Heartbeat" , 
		configMINIMAL_STACK_SIZE, //stack depth
		NULL,//parameters
		1,//priority
		NULL);
	//xTaskCreate( task_Test ,	"task_Test" , configMINIMAL_STACK_SIZE, NULL , 3 , NULL );
	xTaskCreate( task_UART2Recv ,	"task_UART2Recv" , configMINIMAL_STACK_SIZE, NULL , 4 , NULL );	
	//xTaskCreate( task_RefreshRelay ,	"task_RefreshRelay" , configMINIMAL_STACK_SIZE, NULL , 3 , NULL );	
}
