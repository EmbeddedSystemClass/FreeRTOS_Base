#include "stdlib.h"
#include "sys.h"
#include "delay.h"
#include "drv_usart1.h"
#include "drv_usart2.h"
//#include "drv_usart.h"
#include "drv_led.h"
#include "FreeRTOS.h"
#include "task.h"
//#include "sfud.h"
#include "drv_flash.h"
#include "key.h"
#include "easyflash.h"

#define MY_DEBUGF

#ifdef MY_DEBUGF  
#define MY_DEBUG(message,...) do { \
                                  {UARTprintf(message,##__VA_ARGS__);}\
                               } while(0)  
#else    
#define MY_DEBUG(message,...)    
#endif /* PLC_DEBUG */



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
#define KEY_STK_SIZE 		1024
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

static void COMProcessHelp(void);
void flashtest(u8    flag);
void test_env(void);


int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
	delay_init(168);		//��ʼ����ʱ����
	drv_Usart1Init(115200);     	//��ʼ������
	drv_Usart2Init(115200);     	//��ʼ������	
	drv_FlashInit();
//    drv_UartInit();

//    sfud_init();
    
	drv_LED_Init();		        //��ʼ��LED�˿�	
//    KEY_Init();
    bsp_InitKey();

    easyflash_init();

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

    //���̲���
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
		bsp_KeyScan();
        vTaskDelay(10);
    }
}



//FLASH����
void flashtest(u8    flag )
{
    uint8_t data[4096] = {0};    
    size_t size = sizeof(data);
    uint32_t addr = 0;
    size_t i;

    printf("-----flash test start---------\r\n");


    switch(flag)
    {
        case 1:
            for (i = 0; i < size; i++) {
                data[i] = i;
            }

            drv_FLASH_Write((u8*)data,addr,size);		
            vTaskDelay(200);
            memset(data,0x00,size);
            drv_FLASH_Read(data,addr,size);	

            printf("Offset (h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n");

            for (i = 0; i < size; i++) {
                if (i % 16 == 0) {
                    printf("[%08X] ", addr + i);
                }
                printf("%02X ", data[i]);
                if (((i + 1) % 16 == 0) || i == size - 1) {
                    printf("\r\n");
                }
            }
            printf("\r\n");

            printf("-----flash test end---------\r\n");
            break;
       case 2:
            memset(data,0x55,size);
            drv_FLASH_Write((u8*)data,addr,100);
            vTaskDelay(200);
            drv_FLASH_Read(data,addr,100);	
                   for (i = 0; i < 100; i++) {
                if (i % 16 == 0) {
                    printf("[%08X] ", addr + i);
                }
                printf("%02X ", data[i]);
                if (((i + 1) % 16 == 0) || i == 100 - 1) {
                    printf("\r\n");
                }
            }
            printf("\r\n");

            break;           
            
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
					COMProcessHelp();
					break;
				case 3:
					flashtest(1);
					break;
				case 4:
                    flashtest(2);
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
		ucKeyCode = bsp_GetKey();
		
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

					break;
				
				/* K2�����£���ӡ���ڲ������� */
				case KEY_DOWN_K2:
					COMProcessHelp();
					break;
				case KEY_DOWN_K3:
					flashtest(1);
					break;
				case KEY_DOWN_K4:
					test_env();
                    break;
				case SYS_DOWN_K1K2:
					printf("��ϰ���\r\n");                   
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



#ifdef SFUD
void flash_task(void *pvParameters)
{
    uint8_t data[1024] = {0};    
    sfud_err result = SFUD_SUCCESS;    
    size_t size = 1024;
    uint32_t addr = 0;
    size_t i;
    
//    if(sfud_device_init(&gMyFlash) != SFUD_SUCCESS)
//    {
//        printf("flash initial error!\r\n");
//        return;
//    } 
    const sfud_flash * myFlash = sfud_get_device_table() + 0;
    
    /* prepare write data */
    for (i = 0; i < size; i++) {
        data[i] = i;
    }
    /* erase test */
    result = sfud_erase(myFlash, addr, size);
    if (result == SFUD_SUCCESS) {
        printf("Erase the %s flash data finish. Start from 0x%08X, size is %ld.\r\n", myFlash->name, addr,
                size);
    } else {
        printf("Erase the %s flash data failed.\r\n", myFlash->name);
        return;
    }
    /* write test */
    result = sfud_write(myFlash, addr, size, data);
    if (result == SFUD_SUCCESS) {
        printf("Write the %s flash data finish. Start from 0x%08X, size is %ld.\r\n", myFlash->name, addr,
                size);
    } else {
        printf("Write the %s flash data failed.\r\n", myFlash->name);
        return;
    }
    /* read test */
    result = sfud_read(myFlash, addr, size, data);
    if (result == SFUD_SUCCESS) {
        printf("Read the %s flash data success. Start from 0x%08X, size is %ld. The data is:\r\n", myFlash->name, addr,
                size);
        printf("Offset (h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n");
        for (i = 0; i < size; i++) {
            if (i % 16 == 0) {
                printf("[%08X] ", addr + i);
            }
            printf("%02X ", data[i]);
            if (((i + 1) % 16 == 0) || i == size - 1) {
                printf("\r\n");
            }
        }
        printf("\r\n");
    } else {
        printf("Read the %s flash data failed.\r\n", myFlash->name);
    }
    
    /* data check */
    for (i = 0; i < size; i++) {
        if (data[i] != i % 256) {
            printf("Read and check write data has an error. Write the %s flash data failed.\r\n", myFlash->name);
			break;
        }
    }

    if (i == size) {
        printf("The %s flash test is success.\r\n", myFlash->name);
    }    
}
#endif

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

static void COMProcessHelp(void)
{
	printf("==========================================\r\n");
	printf("��ѡ�񴮿ڲ���������Լ��̴�ӡ���ּ���:\r\n");
	printf("1. ���յ�����1����ӡ��Ϣ\r\n");
	printf("2. ���յ�����2����ӡ��Ϣ\r\n");
	printf("3. ���յ�����3����ӡ��Ϣ\r\n");
	printf("4. ���յ�����4����ӡ��Ϣ\r\n");
}

void test_env(void)
{
     uint32_t i_boot_times = NULL;
    char *c_old_boot_times, c_new_boot_times[11] = {0};

    /* get the boot count number from Env */
    c_old_boot_times = ef_get_env("boot_times");
    assert_param(c_old_boot_times);
    i_boot_times = atol(c_old_boot_times);
    /* boot count +1 */
    i_boot_times ++;
    printf("The system now boot %d times\r\n", i_boot_times);
    /* interger to string */
    sprintf(c_new_boot_times,"%ld", i_boot_times);
    /* set and store the boot count number to Env */
    ef_set_env("boot_times", c_new_boot_times);
    ef_save_env();

    ef_set_and_save_env("p1","wangwu");
    ef_set_and_save_env("p2","zhangsan");
    ef_set_and_save_env("p3","lisi");

    ef_save_env();

    c_old_boot_times = ef_get_env("p1");



    UARTprintf("�����ǣ�    %s\r\n", c_old_boot_times);

    c_old_boot_times = ef_get_env("p2");

    MY_DEBUG("�����ǣ�    %s\r\n", c_old_boot_times);

    c_old_boot_times = ef_get_env("p3");

    MY_DEBUG("�����ǣ�    %s\r\n", c_old_boot_times);       

    

    MY_DEBUG("------------test_env end----------\r\n");
}



