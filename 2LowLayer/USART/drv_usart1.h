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

//���Ŷ���
/*******************************************************/
#define USART1_CLK                         RCC_APB2Periph_USART1
#define USART1_GPIO_PORT                   GPIOA
#define USART1_GPIO_CLK                    RCC_AHB1Periph_GPIOA

#define USART1_RX_PIN                      GPIO_Pin_10
#define USART1_RX_AF                       GPIO_AF_USART1
#define USART1_RX_SOURCE                   GPIO_PinSource10

#define USART1_TX_PIN                      GPIO_Pin_9
#define USART1_TX_AF                       GPIO_AF_USART1
#define USART1_TX_SOURCE                   GPIO_PinSource9



#if 0

#include "stdio.h"	
#include "string.h"
#include "stm32f4xx_conf.h"
#include "stdarg.h"


#define USART1MAXBUFFSIZE 1280 //��Ҫ��IAP������һ��1024���ֽ�

extern volatile u8 USART1RecvBuf[USART1MAXBUFFSIZE];
extern volatile u16 RecvTop1;
extern volatile u16 RecvEnd1;



void drv_Usart1Init (u32 BaudRate);
void drv_Usart1SendString (const u8 *Buff);
void drv_Usart1SendData (const u8 *Buff, u16 SendSize);
void drv_Usart1RecvReset (void);
u8 drv_Usart1RecvOne (u8 *Str);
u16 drv_Usart1RecvAtTime (u8 *Buff, u16 RecvSize, u32 timeout_MilliSeconds);
u8 drv_Usart1Read(u8 *buf, u16 len);

//void UARTprintf(const uint8_t *pcString, ...);
#endif


void drv_Usart1_Init(int BaudRate);
void drv_Usart1_DMA_Send(uint8_t *buf, uint16_t len);
uint8_t drv_Usart1_GetByte(void);



    
#endif


