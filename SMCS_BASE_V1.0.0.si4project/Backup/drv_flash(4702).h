/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : drv_flash.h
  �� �� ��   : ����
  ��    ��   : �Ŷ�
  ��������   : 2019��5��23��
  ����޸�   :
  ��������   : �ⲿFLASH��д����
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2019��5��23��
    ��    ��   : �Ŷ�
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef __DRV_FLASH_H
#define __DRV_FLASH_H



void drv_FlashInit (void);



unsigned char drv_FlashSendByte (unsigned char byte);



void drv_FlashEraseSector (unsigned long SectorAddr);


void drv_FlashEraseBulk (void);




void drv_FlashWriteEnable (void);


void drv_FlashWaitForWriteEnd (void);



void drv_FlashWriteBuffer (const unsigned char* pBuffer, unsigned long WriteAddr, unsigned int NumByteToWrite);


void drv_FlashWritePage (const unsigned char* pBuffer, unsigned long WriteAddr, unsigned int NumByteToWrite);





unsigned long drv_FlashReadID (void);


void drv_FlashReadBuffer (unsigned char* pBuffer, unsigned long ReadAddr, unsigned int NumByteToRead);

void drv_FlashEraseBlock(unsigned long SectorAddr);


#endif



