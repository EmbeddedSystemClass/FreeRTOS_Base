#include "stdlib.h"
#include "sys.h"
#include "delay.h"
#include "drv_usart1.h"
#include "drv_usart2.h"
//#include "drv_usart.h"
#include "drv_led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sfud.h"
//#include "drv_flash.h"
#include "drv_key.h"
#include "easyflash.h"
#include "drv_time.h"

#include "ini.h"
#include "test.h"


//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		128
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//�������ȼ�
#define LED0_TASK_PRIO		2
//�����ջ��С	
#define LED0_STK_SIZE 		50  
//������
TaskHandle_t LED0Task_Handler;
//������
void led0_task(void *pvParameters);

//�������ȼ�
#define LED1_TASK_PRIO		3
//�����ջ��С	
#define LED1_STK_SIZE 		50  
//������
TaskHandle_t LED1Task_Handler;
//������
void led1_task(void *pvParameters);

//�������ȼ�
#define VKEY_TASK_PRIO		4
//�����ջ��С	
#define VKEY_STK_SIZE 		128
//������
TaskHandle_t VKEYTask_Handler;
//������
void vkeyTaskStart(void *pvParameters);


//�������ȼ�
#define KEY_TASK_PRIO		5
//�����ջ��С	
#define KEY_STK_SIZE 		1024*2
//������
TaskHandle_t KEYTask_Handler;
//������
void Key_task(void *pvParameters);

//�������ȼ�
#define CMD_TASK_PRIO		6
//�����ջ��С	
#define CMD_STK_SIZE 		128
//������
TaskHandle_t CMDTask_Handler;
//������
void vTaskMsgPro(void *pvParameters);




int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
	delay_init(168);		//��ʼ����ʱ����

    //  drv_FlashInit();
    //  drv_UartInit();
    //  KEY_Init();
    //  sfud_init();
    
	drv_Usart1Init(115200);     	//��ʼ������
	drv_Usart2Init(115200);     	//��ʼ������	
	drv_LED_Init();		            //��ʼ��LED�˿�	
    drv_TIM6_Init();                //��ʱ��6��ʼ��
    easyflash_init();
    drv_InitKey();
    
    

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
    xTaskCreate((TaskFunction_t )vkeyTaskStart,     
                (const char*    )"vkeyTaskStart",   
                (uint16_t       )VKEY_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )VKEY_TASK_PRIO,
                (TaskHandle_t*  )&VKEYTask_Handler);                       
    
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

void vkeyTaskStart(void *pvParameters)
{
    while(1)
    {
		/* ����ɨ�� */
		drv_KeyScan();
        vTaskDelay(10);
    }
}

void Key_task(void *pvParameters)
{
    
	uint8_t ucKeyCode;
	uint8_t pcWriteBuffer[500];
    
#if V5 == 0
    while(1)
    {
		ucKeyCode = KEY_Scan(0);
		
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
					break;
				
				/* K2�����£���ӡ���ڲ������� */
				case 2:
					FlashTest();
					break;
				case 3:
					ef_print_env();
					break;
				case 4:
                    test_env();
					break;                
				
				/* �����ļ�ֵ������ */
				default:                     
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
                    FlashTest();
					break;
				case KEY_DOWN_K3:
                    ef_print_env();
					break;
				case KEY_DOWN_K4:
					test_env();
                    break;
				case SYS_DOWN_K1K2:
					printf("��ϰ���\r\n");    
                    RestoreDefaultSetting();
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
	char buf[64];

	/* ��ӡ�������������ʾ */
	COMProcessHelp();
	
    while(1)
    {
		if (drv_Usart1RecvOne(&ucCMD))
		{
			switch (ucCMD)
			{
				case '1':
					sprintf(buf, "���յ���������1\r\n");	
					drv_Usart2SendData((uint8_t *)buf, strlen(buf));
					break;

				case '2':
					sprintf(buf, "���յ���������2\r\n");
					drv_Usart2SendData((uint8_t *)buf, strlen(buf));
					break;

				case '3':
					sprintf(buf, "���յ���������3\r\n");
					drv_Usart2SendData((uint8_t *)buf, strlen(buf));
					break;

				case '4':
					sprintf(buf, "���յ���������4\r\n");
					drv_Usart2SendData( (uint8_t *)buf, strlen(buf));
					break;	
			}
		}
		
		vTaskDelay(20);
    }
}





