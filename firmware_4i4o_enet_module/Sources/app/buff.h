/**
 * Copyright (C), 1993-2016, Hytera Comm. Co., Ltd.
 * @file    buff.h
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
#ifndef __BUFF_H
#define __BUFF_H

#include "stdint.h"

#define BUFF_SIZE 128

typedef struct buffNode
{
	uint8_t Data;
	struct buffNode* Next;
}BuffNode;



void BUFF_Init( void );
uint16_t BUFF_GetCount( void );
void BUFF_Push( uint8_t data);

uint8_t BUFF_Pop( void );

uint8_t BUFF_GetData( uint16_t index );

void BUFF_Remove( uint16_t count );

//INT16 BUFF_find(UINT16 start, UINT8* dest, UINT16 dest_len);

#endif

