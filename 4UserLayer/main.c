//��׼��ͷ�ļ�
#include <string.h>
#include <stdint.h>
#include <stdlib.h>


//FreeRTOSͷ�ļ�
#include "FreeRTOS.h"
#include "task.h"

//������ͷ�ļ�
//#include "drv_flash.h"
//#include "drv_usart.h"
#include "drv_usart1.h"
//#include "drv_usart2.h"
#include "drv_usart3.h"
#include "drv_key.h"
#include "drv_time.h"
#include "drv_led.h"


//�м��ͷ�ļ�
#include "sys.h"
#include "delay.h"

#include "FreeRTOS_CLI.h"
#include "stmflash.h"
#include "easyflash.h"
#include "cmd_fun.h"
#include "sfud.h"
#include "ini.h"


//�û���ͷ�ļ�
#include "test.h"
#include "iap.h"


//�������ȼ�    
#define CMD_TASK_PRIO		( tskIDLE_PRIORITY + 1)
#define KEY_TASK_PRIO		( tskIDLE_PRIORITY + 4)
#define VKEY_TASK_PRIO		( tskIDLE_PRIORITY + 3)
#define START_TASK_PRIO		( tskIDLE_PRIORITY + 2)
#define LED0_TASK_PRIO		( tskIDLE_PRIORITY + 3)
#define LED1_TASK_PRIO		( tskIDLE_PRIORITY + 3)
#define FLOAT_TASK_PRIO		( tskIDLE_PRIORITY )

//�����ջ��С	
#define START_STK_SIZE 		128  
#define LED0_STK_SIZE 		50 
#define LED1_STK_SIZE 		50  
#define FLOAT_STK_SIZE 		128
#define VKEY_STK_SIZE 		128
#define CMD_STK_SIZE 		1024
#define KEY_STK_SIZE 		1024



//������
TaskHandle_t StartTask_Handler = NULL;
TaskHandle_t LED0Task_Handler = NULL;
TaskHandle_t LED1Task_Handler = NULL;
TaskHandle_t FLOATTask_Handler = NULL;
TaskHandle_t VKEYTask_Handler = NULL;
TaskHandle_t CMDTask_Handler = NULL;
TaskHandle_t KEYTask_Handler = NULL;


//������
void start_task(void *pvParameters);
void led0_task(void *pvParameters);
void led1_task(void *pvParameters);
void float_task(void *pvParameters);
void vkeyTaskStart(void *pvParameters);
void vTaskMsgPro(void *pvParameters);
void Key_task(void *pvParameters);


#define SFUD_DEMO_TEST_BUFFER_SIZE 1024
static void sfud_demo(void);
static uint8_t sfud_demo_test_buf[SFUD_DEMO_TEST_BUFFER_SIZE];




int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
	delay_init(168);		//��ʼ����ʱ����

    //  drv_FlashInit();
    //  drv_UartInit();    
    //  sfud_init();
    //  drv_InitKey();

    
    STM_FLASH_Init();               //оƬ�ڲ�FLASH��ʼ��
    
	drv_Usart1_Init(115200);     	//��ʼ������
	xUsart2Init(115200);            //drv_Usart2_Init(115200);     	//��ʼ������	
//	drv_Usart3_Init(115200);     	//��ʼ������	
	
	drv_key_Init();                 //������ʼ��
	drv_LED_Init();		            //��ʼ��LED�˿�	
    drv_TIM6_Init();                //��ʱ��6��ʼ��
    easyflash_init();               //�ⲿFLASH��ʼ��
    CMD_Init();                     //FreeRTOS CLI ע��
    

	//������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,        //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������              
    vTaskStartScheduler();          //�����������
}
 
//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���

    vUARTCommandConsoleStart(1024,4);
    
    //����LED0����
    xTaskCreate((TaskFunction_t )led0_task,     	
                (const char*    )"led0_task",   	
                (uint16_t       )LED0_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )LED0_TASK_PRIO,	
                (TaskHandle_t*  )&LED0Task_Handler);   
    //����LED1����
    xTaskCreate((TaskFunction_t )led1_task,     
                (const char*    )"led1_task",   
                (uint16_t       )LED1_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LED1_TASK_PRIO,
                (TaskHandle_t*  )&LED1Task_Handler);  

    //����ɨ��
//    xTaskCreate((TaskFunction_t )vkeyTaskStart,     
//                (const char*    )"vkeyTaskStart",   
//                (uint16_t       )VKEY_STK_SIZE, 
//                (void*          )NULL,
//                (UBaseType_t    )VKEY_TASK_PRIO,
//                (TaskHandle_t*  )&VKEYTask_Handler);                       
    
    //���̲���
    xTaskCreate((TaskFunction_t )Key_task,     
                (const char*    )"Key_task",   
                (uint16_t       )KEY_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )KEY_TASK_PRIO,
                (TaskHandle_t*  )&KEYTask_Handler);                 
                

    //CMD��������
    xTaskCreate((TaskFunction_t )vTaskMsgPro,     
                (const char*    )"vTaskMsgPro",   
                (uint16_t       )CMD_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )CMD_TASK_PRIO,
                (TaskHandle_t*  )&CMDTask_Handler);                  

                
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����	
				
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//LED0������ 
void led0_task(void *pvParameters)
{
    while(1)
    {
        LED0=~LED0;
        vTaskDelay(500);
    }
}   

//LED1������
void led1_task(void *pvParameters)
{
    while(1)
    {
        LED1=0;
        vTaskDelay(200);
        LED1=1;
        vTaskDelay(800);
    }
}

//void vkeyTaskStart(void *pvParameters)
//{
//    while(1)
//    {
//		/* ����ɨ�� */
//		drv_KeyScan();
//        vTaskDelay(10);
//    }
//}

void Key_task(void *pvParameters)
{
    
	uint8_t ucKeyCode;
	uint8_t pcWriteBuffer[500];
    
#if V5 == 0
    while(1)
    {
		ucKeyCode = drv_key_Scan(0);
		
		if (ucKeyCode != 0)
		{
			switch (ucKeyCode)
			{
				/* K1������ ��ӡ����ִ����� */
				case 1:			 
					printf("=================================================\r\n");
					printf("������      ����״̬ ���ȼ�   ʣ��ջ �������\r\n");
					vTaskList((char *)&pcWriteBuffer);
					printf("%s\r\n", pcWriteBuffer);
                    
					printf("\r\n������       ���м���         ʹ����\r\n");
					vTaskGetRunTimeStats((char *)&pcWriteBuffer);
					printf("%s\r\n", pcWriteBuffer);                    
					break;				
				/* K2�����£���ӡ���ڲ������� */
				case 2:
					//FlashTest();
					sfud_demo();
					break;
				case 3:
					ef_print_env();
					break;
				case 4:
                    printf("KEY_DOWN_K4\r\n");
                    //RestoreDefaultSetting();
                    IAP_DownLoadToFlash();
					break;                
				
				/* �����ļ�ֵ������ */
				default:   
				printf("KEY_default\r\n");
					break;
			}
		}
		
		vTaskDelay(20);
	}
    
    #else
    
    while(1)
    {
		ucKeyCode = drv_GetKey();
		
		if (ucKeyCode != KEY_NONE)
		{
			switch (ucKeyCode)
			{
				/* K1������ ��ӡ����ִ����� */
				case KEY_DOWN_K1:			 
					printf("=================================================\r\n");
					printf("������      ����״̬ ���ȼ�   ʣ��ջ �������\r\n");
					vTaskList((char *)&pcWriteBuffer);
					printf("%s\r\n", pcWriteBuffer);
                    
					printf("\r\n������       ���м���         ʹ����\r\n");
					vTaskGetRunTimeStats((char *)&pcWriteBuffer);
					printf("%s\r\n", pcWriteBuffer);
					break;

				
				/* K2�����£���ӡ���ڲ������� */
				case KEY_DOWN_K2: 
                    //FlashTest();
                    sfud_demo();
					break;
				case KEY_DOWN_K3:
                    ef_print_env();
					break;
				case KEY_DOWN_K4:
                    
                    printf("KEY_DOWN_K4\r\n");
                    break;
				case SYS_DOWN_K1K2:
					printf("��ϰ���\r\n");    
                    //RestoreDefaultSetting();
					break;                
				
				/* �����ļ�ֵ������ */
				default:                     
					break;
			}
		}
		
		vTaskDelay(20);
	}

    #endif
}


void vTaskMsgPro(void *pvParameters)
{
	uint8_t ucCMD;	
    
    while(1)
    {
        ucCMD = drv_Usart1_GetByte();
        
		switch (ucCMD)
		{
			case '1':
                printf( "���յ���������1\r\n");	               
				break;

			case '2':
				printf( "���յ���������2\r\n");					
				break;

			case '3':
				printf( "���յ���������3\r\n");				
				break;

			case '4':
				printf( "���յ���������4\r\n");					
				break;	
		}
	
		vTaskDelay(20);
    }
}


static void sfud_demo(void) {
    sfud_err result = SFUD_SUCCESS;
    const sfud_flash *flash = sfud_get_device_table() + 0;
    size_t i;
    uint32_t addr = 0x10000;
    size_t size = SFUD_DEMO_TEST_BUFFER_SIZE;
    size_t j = 0;

    for(j=0;j<43;j++,addr+=SFUD_DEMO_TEST_BUFFER_SIZE)
    {
        
        result = sfud_read(flash, addr, size, sfud_demo_test_buf);
        if (result == SFUD_SUCCESS) 
        {
            printf("Read the %s flash data success. Start from 0x%08X, size is %ld. The data is:\r\n", flash->name, addr,
                    size);
            printf("Offset (h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n");
            for (i = 0; i < size; i++) 
            {
                if (i % 16 == 0) 
                {
                    printf("[%08X] ", addr + i);
                }
                printf("%02X ", sfud_demo_test_buf[i]);
                if (((i + 1) % 16 == 0) || i == size - 1) 
                {
                    printf("\r\n");
                }
            }
            printf("\r\n");
        } 
        else 
        {
            printf("Read the %s flash data failed.\r\n", flash->name);
        }
    }
    if (i == size) {
        printf("The %s flash test is success.\r\n", flash->name);
    }
}




