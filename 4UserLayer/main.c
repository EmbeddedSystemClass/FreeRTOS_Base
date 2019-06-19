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
#include "comm.h"



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

    init_serial_boot();
    

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
                (const char*    )"cmd",   
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
					ReadIAP();
					break;
				case 3:
					//ef_print_env();
					//SystemReset();
					break;
				case 4:
                    printf("KEY_DOWN_K4\r\n");
                    drv_Usart1_SendString("1234");
                    //ef_erase_bak_app( 0x10000 ); 
                    //RestoreDefaultSetting();
                    //IAP_DownLoadToFlash();
					break;                
				
				/* �����ļ�ֵ������ */
				default:   
				printf("KEY_default\r\n");
					break;
			}
		}
		
		vTaskDelay(20);
	}   

}


void vTaskMsgPro(void *pvParameters)
{
    
    while(1)
    {
        DealSerialParse();    
	    DealRxData();
		vTaskDelay(10);
    }
}



