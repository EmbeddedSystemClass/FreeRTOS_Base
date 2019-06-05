/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : drv_usart2.h
  �� �� ��   : ����
  ��    ��   : �Ŷ�
  ��������   : 2019��5��22��
  ����޸�   :
  ��������   : ����2 ָ�����
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2019��5��22��
    ��    ��   : �Ŷ�
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef __DRV_USART2_H
#define __DRV_USART2_H

#include "stdio.h"	
#include "string.h"
#include "stm32f4xx_conf.h"



#define USART2MAXBUFFSIZE 1024

void drv_Usart2_Init (u32 BaudRate);
void drv_Usart2SendString (const u8 *Buff);
void drv_Usart2SendData (const u8 *Buff, u16 SendSize);
void drv_Usart2RecvReset (void);
u8 drv_Usart2RecvOne (u8 *Str);
u16 drv_Usart2RecvAtTime (u8 *Buff, u16 RecvSize, u32 timeout_MilliSeconds);
u8 drv_Usart2Read(u8 *buf, u16 len);

    
#endif


