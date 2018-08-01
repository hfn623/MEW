/**
 * Copyright (C), 1993-2016, Hytera Comm. Co., Ltd.
 * @file    buff.c
 * @brief   环形队列缓冲区
 * @author  滑福宁
 * @version v1.0
 * @date    2016-8-23
 */
/*
 *   函数列表
 *
 *   01. BUFF_init								初始化
 *   02. BUFF_count								获取
 *   03. BUFF_push								入列
 *   04. BUFF_pop									出列
 *   05. BUFF_get									取某一位置数据
 *   06. BUFF_remove							删除
 *   07. BUFF_find								查找
 *   History:
 *   2016-08-23		滑福宁		创建
 *   *************************************************************************/


#include "buff.h"

static BuffNode buff[BUFF_SIZE];

static BuffNode * pPush;

static BuffNode * pPop;

static uint16_t buff_count=0;
//static UINT8 isPushing=0,isPoping=0,isRemoving=0;


//int8_t BUFF_check( uint16_t * oi , uint16_t* oj )
//{
//	int16_t i,j;
//	for( i = 0 ; i < BUFF_SIZE ; i ++ )
//	{
//		for( j = 0 ; j < BUFF_SIZE ; j ++ )
//		{
//			if( i != j )
//			{
//				if( BUFF[ i ].Next == BUFF[ j ].Next )
//				{
//					* oi = i;
//					* oj = j;
//					return 0;
//				}
//			}
//		}
//	}
//	return -1;
//}
/**
 * @brief				BUFF初始化，构造
 *
 * @param[in]		无
 *
 * @return			无
 * @author			滑福宁
 * @bug  
 */
void BUFF_Init( void )
{
	uint16_t i=0;
	for( i = 0 ; i < BUFF_SIZE - 1 ; i++ )
	{
		buff[ i ].Data = 0;
		buff[ i ].Next = & ( buff [ i + 1 ] );
	}
	buff[ i ].Data = 0;
	buff[ i ].Next = & ( buff[ 0 ] );
	
	pPush = & ( buff[ 0 ] );
	pPop = & ( buff[ 0 ] );
	
	buff_count = 0;
	
//	isPushing=0;
//	isPoping=0;
//	isRemoving=0;
}
/**
 * @brief				获取BUFF内容个数
 *
 * @param[in]		无
 *
 * @return			无
 * @author			滑福宁
 * @bug  
 */
uint16_t BUFF_GetCount( void )
{
	return buff_count;
}
/**
 * @brief				入队列
 *
 * @param[in]		data入队数据
 *
 * @return			无
 * @author			滑福宁
 * @bug  
 */
void BUFF_Push( uint8_t data )
{
	
	if( buff_count < BUFF_SIZE )
	{
//		while(isRemoving);
//		isPushing=1;
		//pPushPre=pPush;
		pPush -> Data = data;
		pPush = pPush -> Next;
		buff_count ++;
//		isPushing=0;

	}
}
/**
 * @brief				出队列
 *
 * @param[out]		出队列数据
 *
 * @return			无
 * @author			滑福宁
 * @bug  
 */
uint8_t BUFF_Pop( void )
{
	uint8_t data = pPop -> Data;
	if( buff_count > 0 )
	{
//		while(isRemoving);
//		isPoping=1;
		pPop = pPop -> Next;
		buff_count --;
//		isPoping=0;
	}
	else
	{
		//todo error
	}
	return data;
}
/**
 * @brief				读取某一位置数据
 *
 * @param[in]		index 相对于开始位置
 * @param[out]	得到的数据
 *
 * @return			无
 * @author			滑福宁
 * @bug  
 */
uint8_t BUFF_GetData( uint16_t index )
{
	uint16_t i=0;
	uint8_t data;
	BuffNode * pTmp = pPop;
	for( i = 0 ; i < index ; i ++ )
	{
		pTmp = pTmp -> Next;
	}
	data = pTmp -> Data;
	return data;
}
/**
 * @brief				删除数据
 *
 * @param[in]		从头开始，删除个数
 * @param[out]	无
 *
 * @return			无
 * @author			滑福宁
 * @bug  
 */
void BUFF_Remove( uint16_t count )
{
	int16_t i=0;

	
//	while(isPushing||isPoping);
//	isRemoving=1;
		for( i = 0 ; i < count ; i ++ )
		{
			pPop = pPop -> Next;
		}
	
	if( buff_count >= count )
	{
		buff_count -= count;
	}
	else
	{
		//todo error
	}
//	isRemoving=0;		
}

