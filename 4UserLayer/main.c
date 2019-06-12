//��׼��ͷ�ļ�
#include <string.h>
#include <stdint.h>
#include <stdlib.h>


//FreeRTOSͷ�ļ�
#include "FreeRTOS.h"
#include "task.h"

//������ͷ�ļ�
#include "drv_usart1.h"
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
#include "iap.h"


//�������ȼ�    
#define KEY_TASK_PRIO		( tskIDLE_PRIORITY + 4)
#define LED0_TASK_PRIO		( tskIDLE_PRIORITY + 3)
#define LED1_TASK_PRIO		( tskIDLE_PRIORITY + 3)
#define CheckUP_TASK_PRIO	( tskIDLE_PRIORITY + 2)
#define START_TASK_PRIO		( tskIDLE_PRIORITY )

//�����ջ��С	
#define START_STK_SIZE 		128  
#define LED0_STK_SIZE 		50 
#define LED1_STK_SIZE 		50  
#define KEY_STK_SIZE 		1024
#define CheckUP_STK_SIZE 		1024




//������
TaskHandle_t StartTask_Handler = NULL;
TaskHandle_t LED0Task_Handler = NULL;
TaskHandle_t LED1Task_Handler = NULL;
TaskHandle_t KEYTask_Handler = NULL;
TaskHandle_t CheckUP_Handler = NULL;





//������
void start_task(void *pvParameters);
void led0_task(void *pvParameters);
void led1_task(void *pvParameters);
void Key_task(void *pvParameters);
void CheckUP_task(void *pvParameters);




int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
	delay_init(168);		        //��ʼ����ʱ����
	
    drv_TIM6_Init();                //��ʱ��6��ʼ��    
    STM_FLASH_Init();               //оƬ�ڲ�FLASH��ʼ��
    
	drv_Usart1_Init(115200);     	//��ʼ������ YMODEMʹ��
	xUsart2Init(115200);            //USART2��ʼ����FreeRTOS CLI ʹ��
//	drv_Usart3_Init(115200);     	//��ʼ������3	
	drv_key_Init();                 //������ʼ��
	easyflash_init();               //�ⲿFLASH��ʼ��
    CMD_Init();                     //FreeRTOS CLI ע��
    
	drv_LED_Init();		            //��ʼ��LED�˿�	
    

    

	//������ʼ����
    xTaskCreate((TaskFunction_t )CheckUP_task,            //������
                (const char*    )"CheckUP_task",          //��������
                (uint16_t       )CheckUP_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )CheckUP_TASK_PRIO,        //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������              
    vTaskStartScheduler();          //�����������
}
 
//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���

    //����LED0����
    xTaskCreate((TaskFunction_t )led0_task,       
                (const char*    )"led0_task",     
                (uint16_t       )LED0_STK_SIZE, 
                (void*          )NULL,                
                (UBaseType_t    )CheckUP_TASK_PRIO,  
                (TaskHandle_t*  )&CheckUP_Handler);   




//    vUARTCommandConsoleStart(1024,4); //FreeRTOS CLI
    
//    //����LED0����
//    xTaskCreate((TaskFunction_t )led0_task,     	
//                (const char*    )"led0_task",   	
//                (uint16_t       )LED0_STK_SIZE, 
//                (void*          )NULL,				
//                (UBaseType_t    )LED0_TASK_PRIO,	
//                (TaskHandle_t*  )&LED0Task_Handler);   
//    //����LED1����
//    xTaskCreate((TaskFunction_t )led1_task,     
//                (const char*    )"led1_task",   
//                (uint16_t       )LED1_STK_SIZE, 
//                (void*          )NULL,
//                (UBaseType_t    )LED1_TASK_PRIO,
//                (TaskHandle_t*  )&LED1Task_Handler);                  
//    
//    //���̲���
//    xTaskCreate((TaskFunction_t )Key_task,     
//                (const char*    )"Key_task",   
//                (uint16_t       )KEY_STK_SIZE, 
//                (void*          )NULL,
//                (UBaseType_t    )KEY_TASK_PRIO,
//                (TaskHandle_t*  )&KEYTask_Handler);    

                
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

}


void CheckUP_task(void *pvParameters)
{
    uint32_t upValue1 = 0;
    uint32_t upValue2 = 0;
    char *UpgradeFlag1;
    char *UpgradeFlag2;

    while(1)
    {   

        UpgradeFlag1 = ef_get_env("UpgradeFlag");

        upValue1 = atol(UpgradeFlag1);

        UpgradeFlag2 = ef_get_env("UserAPPFlag");

        upValue2 = atol(UpgradeFlag2);
        
        if(upValue1 == 0 && upValue2 == 1)
        {
            //����Ҫ����������APP
			printf("APP STATR!\r\n");
			IAP_JumpToApplication();
        }
        else
        {
			printf("IAP STATR! %d   %d\r\n",upValue1,upValue2);+

            //����ⲿFLASHд�꣬�����ڲ�FLASHûд������д�ڲ�FLASHʱ�ϵ磩
            
            //��Ҫ����������IAP��������
            IAP_DownLoadToFlash();

            printf("write stm flash\r\n");
            IAP_DownLoadToSTMFlash();
        }
        

        vTaskDelay(100);
    }
}





