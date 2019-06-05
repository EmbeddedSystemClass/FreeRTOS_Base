/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : drv_Usart3.c
  �� �� ��   : ����
  ��    ��   : �Ŷ�
  ��������   : 2019��5��22��
  ����޸�   :
  ��������   : ����3 IAP����
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2019��5��22��
    ��    ��   : �Ŷ�
    �޸�����   : �����ļ�

******************************************************************************/

#include "drv_usart3.h"

volatile u8 USART3RecvBuf[USART3MAXBUFFSIZE] = {0};
volatile u16 RecvTop3 = 0;
volatile u16 RecvEnd3 = 0;



/*****************************************************************************
 �� �� ��  : drv_Usart3Init
 ��������  : ���ڳ�ʼ��
 �������  : u32 BaudRate  ������
 �������  : ��
 �� �� ֵ  : 
 
 �޸���ʷ      :
  1.��    ��   : 2019��5��22��
    ��    ��   : �Ŷ�
void drv_Usart3Init (u32 BaudRate);
    �޸�����   : �����ɺ���

*****************************************************************************/
void drv_Usart3_Init (u32 BaudRate)
{
    //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(USART3_GPIO_CLK,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(USART3_CLK,ENABLE);//ʹ��Usart3ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(USART3_GPIO_PORT,USART3_TX_SOURCE,USART3_TX_AF); //GPIOB10����ΪUSART3
	GPIO_PinAFConfig(USART3_GPIO_PORT,USART3_RX_SOURCE,USART3_RX_AF); //GPIOB11����ΪUSART3
	
	//Usart2�˿�����
	GPIO_InitStructure.GPIO_Pin = USART3_TX_PIN | USART3_RX_PIN; //GPIOB10��GPIOB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(USART3_GPIO_PORT,&GPIO_InitStructure); //��ʼ��PB10��PB11

   //USART3 ��ʼ������
	USART_InitStructure.USART_BaudRate = BaudRate;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART3, &USART_InitStructure); //��ʼ������2    

	//USART3 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//����3�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//��������ж�    
	
	USART_Cmd(USART3, ENABLE);  //ʹ�ܴ���1   

}

void USART3_IRQHandler (void)
{
	u8 temp = 0;

	//Receive data register not empty flag
	if (USART_GetITStatus (USART3, USART_IT_RXNE) != RESET)
	{
		temp = USART_ReceiveData (USART3);		//get received data

		if ( (RecvEnd3 == (RecvTop3 - 1) ) || ( (RecvTop3 == 0) && (RecvEnd3 == (USART3MAXBUFFSIZE - 1) ) ) )
		{
			//�������
			RecvTop3 = 0;
			RecvEnd3 = 0;
		}
		else
		{
			USART3RecvBuf[RecvEnd3] = temp;
			RecvEnd3++;

			if (RecvEnd3 >= USART3MAXBUFFSIZE) RecvEnd3 = 0;
		}

		/* Clear the UART1 Recvive interrupt */
		USART_ClearITPendingBit (USART3, USART_IT_RXNE);
	}
	else if (USART_GetITStatus (USART3, USART_IT_NE | USART_IT_PE | USART_IT_FE | USART_IT_ORE) != RESET)
	{
		USART_GetITStatus (USART3, USART_IT_NE | USART_IT_PE | USART_IT_FE | USART_IT_ORE);
		USART_ReceiveData (USART3);
	}
}


void drv_Usart3_SendOne (const u8 dat)
{
	USART_SendData (USART3, dat);

	while (USART_GetFlagStatus (USART3, USART_FLAG_TXE) == RESET) {}

	USART_ClearFlag (USART3, USART_FLAG_TXE);

}



/*****************************************************************************
 �� �� ��  : drv_Usart3SendString
 ��������  : �����ַ���
 �������  : const u8 *Buff  �����ַ���
 �������  : ��
 �� �� ֵ  : 
 
 �޸���ʷ      :
  1.��    ��   : 2019��5��22��
    ��    ��   : �Ŷ�
void drv_Usart3SendString (const u8 *Buff);
    �޸�����   : �����ɺ���

*****************************************************************************/
void drv_Usart3_SendString (const u8 *Buff)
{
	while (*Buff != 0)
	{
		drv_Usart3_SendOne (*Buff);
		Buff++;
	}    
}


/*****************************************************************************
 �� �� ��  : drv_Usart3SendData
 ��������  : ����ָ�������ַ�
 �������  : const u8 *Buff  �������ݵ�buff
             u16 SendSize  �������ݵĳ���  
 �������  : ��
 �� �� ֵ  : 
 
 �޸���ʷ      :
  1.��    ��   : 2019��5��22��
    ��    ��   : �Ŷ�
void drv_Usart3SendData (const u8 *Buff, u16 SendSize);
    �޸�����   : �����ɺ���

*****************************************************************************/
void drv_Usart3_SendData (const u8 *Buff, u16 SendSize)
{
	while (SendSize != 0)
	{
		drv_Usart3_SendOne (*Buff);
		Buff++;
		SendSize--;
	}

}



/*****************************************************************************
 �� �� ��  : drv_Usart3RecvReset
 ��������  : ��մ��ڽ��ջ���
 �������  : void  
 �������  : ��
 �� �� ֵ  : 
 
 �޸���ʷ      :
  1.��    ��   : 2019��5��22��
    ��    ��   : �Ŷ�
void drv_Usart3RecvReset (void);
    �޸�����   : �����ɺ���

*****************************************************************************/
void drv_Usart3_RecvReset (void)
{
	RecvTop3 = 0;
	RecvEnd3 = 0;

	memset((void *)USART3RecvBuf, 0, USART3MAXBUFFSIZE); 
}



/*****************************************************************************
 �� �� ��  : drv_Usart3RecvOne
 ��������  : ��ȡ1���ֽ�
 �������  : u8 *Str  �������ֽڴ洢��Str[0]
 �������  : ��
 �� �� ֵ  : 
 
 �޸���ʷ      :
  1.��    ��   : 2019��5��22��
    ��    ��   : �Ŷ�
u8 drv_Usart3RecvOne (u8 *Str);
    �޸�����   : �����ɺ���

*****************************************************************************/
u8 drv_Usart3_RecvOne (u8 *Str)
{
	if (RecvTop3 == RecvEnd3) return 0;//read nothing

	*Str = USART3RecvBuf[RecvTop3];
	RecvTop3++;

	if (RecvTop3 >= USART3MAXBUFFSIZE) RecvTop3 = 0;

	return 1;//read one

}



/*****************************************************************************
 �� �� ��  : drv_Usart3RecvAtTime
 ��������  : ���մ�������
 �������  : u8 *Buff �������ݴ�ŵĵط�                 
             u16 RecvSize  ���ܶ��ٸ��ֽ�            
             u32 timeout_MilliSeconds  ��ʱ�ĺ�����
 �������  : ��
 �� �� ֵ  : ���ؽ��ܵ����ֽ���
 
 �޸���ʷ      :
  1.��    ��   : 2019��5��22��
    ��    ��   : �Ŷ�
u16 drv_Usart3RecvAtTime (u8 *Buff, u16 RecvSize, u32 timeout_MilliSeconds);
    �޸�����   : �����ɺ���

*****************************************************************************/
u16 drv_Usart3_RecvAtTime (u8 *Buff, u16 RecvSize, u32 timeout_MilliSeconds)
{
//	u16 RecvLen = 0;
//	u8 tmp[1] = {0};

//	if (RecvSize == 0) return 0;

//	g1msTimer5 = timeout_MilliSeconds;

//	while (1)
//	{
//		if (g1msTimer5 == 0) return RecvLen;

//		if (drv_Usart3RecvOne (tmp) == 1) 
//		{
//			Buff[RecvLen++] = tmp[0];
//		}

//		if (RecvLen >= RecvSize) return RecvLen;
//	}
	return 0;
}


/*****************************************************************************
 �� �� ��  : dev_Usart2Read
 ��������  : ����������
 �������  : unsigned char *buf  
             int len             
 �������  : ��
 �� �� ֵ  : 
 
 �޸���ʷ      :
  1.��    ��   : 2019��5��22��
    ��    ��   : �Ŷ�
int dev_Usart2Read(unsigned char *buf, int len)
    �޸�����   : �����ɺ���

*****************************************************************************/
u8 drv_Usart3_Read(u8 *Buff, u16 len)
{

	u16 RecvLen = 0;
	u8 tmp[1] = {0};

	if (len == 0 || Buff == NULL) return 0;

	while (len--)
	{
		if (drv_Usart3_RecvOne (tmp) == 1)
		{
			Buff[RecvLen++] = tmp[0];
		}

		if (RecvLen >= len) return RecvLen;
	}

	return RecvLen;
    
}



