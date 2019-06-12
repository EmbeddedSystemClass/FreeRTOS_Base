/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : iap.c
  �� �� ��   : ����
  ��    ��   : �Ŷ�
  ��������   : 2019��6��4��
  ����޸�   :
  ��������   : ������������
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2019��6��4��
    ��    ��   : �Ŷ�
    �޸�����   : �����ļ�

******************************************************************************/

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include "iap.h"
#include "ymodem.h"
#include <stdio.h>
#include "easyflash.h"
#include "sfud.h"
#include "stmflash.h"

/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
#define READ_BIN_BUFFER_SIZE 2048


typedef  void (*pFunction)(void); 
pFunction Jump_To_Application;

/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/
uint8_t buf_1k[1024] = {0};
static uint32_t file_total_size = 0 ;


/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/

//static void Int2Str(uint8_t* str, int32_t intnum);




//Convert an interger to a string
//str: converted string
//intnum: integer waiting for converting
//static void Int2Str(uint8_t* str, int32_t intnum)
//{
//  uint32_t i, Div = 1000000000, j = 0, Status = 0;

//  for (i = 0; i < 10; i++)
//  {
//    str[j++] = (intnum / Div) + 48;

//    intnum = intnum % Div;
//    Div /= 10;
//    if ((str[j-1] == '0') & (Status == 0))
//    {
//      j = 0;
//    }
//    else
//    {
//      Status++;
//    }
//  }
//}




void IAP_JumpToApplication(void)
{
    uint32_t JumpAddress = 0;//��ת��ַ
    
    if ( ( (* (volatile uint32_t*) APPLICATION_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
    {
		/* Jump to user application */
		JumpAddress = *(volatile uint32_t*) (APPLICATION_ADDRESS + 4);
		Jump_To_Application = (pFunction) JumpAddress;

		/* Initialize user application's Stack Pointer */
		__set_PSP(*(volatile uint32_t*) APPLICATION_ADDRESS);

		Jump_To_Application();	
        
		while(1);
    }
}

//ͨ�����ڽ����ļ���д���ⲿFLASH
int32_t IAP_DownLoadToFlash(void)
{
//    uint8_t Number[10] = {0};
//	int32_t Size = 0;

    printf("------IAP_DownLoadToFlash start----------\n");


    
    file_total_size = Ymodem_Receive(&buf_1k[0],APPLICATION_ADDRESS);

    if (file_total_size > 0)
	{
//		printf("-------------------\n");
//		printf("\n\r Programming Completed Successfully!\n\r----------------------\r\n Name: ");
//		printf((char*)FileName);
//		Int2Str(Number, Size);
//		printf("\n\r Size: ");
//		printf((char*)Number);
//		printf(" Bytes\r\n");

         if(ef_set_env("UpgradeFlag","0") == EF_NO_ERR)
         {
            return file_total_size;
         }
	}

    return 0;
}





int32_t IAP_DownLoadToSTMFlash(void)
{
	int32_t sent=0;
	int32_t rest=file_total_size;
    uint8_t read_buf[READ_BIN_BUFFER_SIZE];
    uint32_t flash_addr = 0x10000;
    uint32_t  user_app_addr,ramsource;
    size_t DATAS_LENGTH = READ_BIN_BUFFER_SIZE;
    sfud_err result = SFUD_SUCCESS;
    const sfud_flash *flash = sfud_get_device_table() + 0;

    
    user_app_addr = APPLICATION_ADDRESS;
    
    //����û�APP����
    STM_FLASH_Erase ( user_app_addr );    

//    //���ⲿFLASH��ȡ����
    while( sent < file_total_size)
    {
        memset (read_buf, 0, sizeof (read_buf) );
        result = sfud_read(flash, flash_addr, DATAS_LENGTH, read_buf);     
        
        if(result != SFUD_SUCCESS)
        {
            //����
            printf("sfud_read error\r\n");
            return 3;        
        }

        ramsource = ( uint32_t ) read_buf;
        
        if ( STM_FLASH_Write (&user_app_addr, ( uint32_t* ) ramsource, ( uint16_t ) DATAS_LENGTH/4 )  != 0 ) //ֱ��дƬ��FLASH
        {
            //����
            printf("STM_FLASH_Write error\r\n");
            return 2;
        }  

        sent += DATAS_LENGTH;
        rest -= DATAS_LENGTH;

        if (rest >= READ_BIN_BUFFER_SIZE) 
        {
            DATAS_LENGTH = READ_BIN_BUFFER_SIZE;
        }
        else 
        {
            //��Ϊ��дFLASH��ʱ����1024�ˣ�����������Ҫд1024���ֽ�
            DATAS_LENGTH = READ_BIN_BUFFER_SIZE;
            //DATAS_LENGTH = rest; //ֻдʣ������
        }

        flash_addr += DATAS_LENGTH;        
    }

    
    //д���־λ
    if(ef_set_env("UserAPPFlag","1") == EF_NO_ERR)
    {

        printf("STM_FLASH_Write success\r\n");
        return 0;
    }   


    return 1;    
}





