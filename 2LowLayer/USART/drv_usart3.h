/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : drv_usart3.h
  �� �� ��   : ����
  ��    ��   : �Ŷ�
  ��������   : 2019��5��22��
  ����޸�   :
  ��������   : ����3 ָ�����
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2019��5��22��
    ��    ��   : �Ŷ�
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef __DRV_USART3_H
#define __DRV_USART3_H

#include "stdio.h"	
#include "string.h"
#include "stm32f4xx_conf.h"

#define USART3_CLK                         RCC_APB1Periph_USART3
#define USART3_GPIO_PORT                   GPIOB
#define USART3_GPIO_CLK                    RCC_AHB1Periph_GPIOB

#define USART3_RX_PIN                      GPIO_Pin_11
#define USART3_RX_AF                       GPIO_AF_USART3
#define USART3_RX_SOURCE                   GPIO_PinSource11


#define USART3_TX_PIN                      GPIO_Pin_10
#define USART3_TX_AF                       GPIO_AF_USART3
#define USART3_TX_SOURCE                   GPIO_PinSource10




#define USART3MAXBUFFSIZE 1024

void drv_usart3Init (u32 BaudRate);
void drv_usart3SendString (const u8 *Buff);
void drv_usart3SendData (const u8 *Buff, u16 SendSize);
void drv_usart3RecvReset (void);
u8 drv_usart3RecvOne (u8 *Str);
u16 drv_usart3RecvAtTime (u8 *Buff, u16 RecvSize, u32 timeout_MilliSeconds);
u8 drv_usart3Read(u8 *buf, u16 len);

    
#endif


