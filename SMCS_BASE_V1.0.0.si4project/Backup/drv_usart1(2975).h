/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : drv_usart1.h
  �� �� ��   : ����
  ��    ��   : �Ŷ�
  ��������   : 2019��5��22��
  ����޸�   :
  ��������   : ����1��ʼ��
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2019��5��22��
    ��    ��   : �Ŷ�
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef __DRV_USART1_H
#define __DRV_USART1_H

#include "stdio.h"	
#include "string.h"
#include "stm32f4xx_conf.h"

#define USART1MAXBUFFSIZE 128

void drv_Usart1Init (u32 BaudRate);
void drv_Usart1SendString (const u8 *Buff);
void drv_Usart1SendData (const u8 *Buff, u16 SendSize);
void drv_Usart1RecvReset (void);
u8 drv_Usart1RecvOne (u8 *Str);
u16 drv_Usart1RecvAtTime (u8 *Buff, u16 RecvSize, u32 timeout_MilliSeconds);
u8 drv_Usart1Read(u8 *buf, u16 len);
    
#endif


