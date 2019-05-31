/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : drv_usart1.c
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
#include "drv_usart1.h"

volatile u8 USART1RecvBuf[USART1MAXBUFFSIZE] = {0};
volatile u16 RecvTop1 = 0;
volatile u16 RecvEnd1 = 0;

#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif



/*****************************************************************************
 �� �� ��  : drv_Usart1Init
 ��������  : ���ڳ�ʼ��
 �������  : u32 BaudRate  ������
 �������  : ��
 �� �� ֵ  : 
 
 �޸���ʷ      :
  1.��    ��   : 2019��5��22��
    ��    ��   : �Ŷ�
void drv_Usart1Init (u32 BaudRate);
    �޸�����   : �����ɺ���

*****************************************************************************/
void drv_Usart1Init (u32 BaudRate)
{
    //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9����ΪUSART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10����ΪUSART1
	
	//USART1�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA9��PA10

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = BaudRate;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������1    

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�    
	
	USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1   

}

void USART1_IRQHandler (void)
{
	u8 temp = 0;

	//Receive data register not empty flag
	if (USART_GetITStatus (USART1, USART_IT_RXNE) != RESET)
	{
		temp = USART_ReceiveData (USART1);		//get received data

		if ( (RecvEnd1 == (RecvTop1 - 1) ) || ( (RecvTop1 == 0) && (RecvEnd1 == (USART1MAXBUFFSIZE - 1) ) ) )
		{
			//�������
			RecvTop1 = 0;
			RecvEnd1 = 0;
		}
		else
		{
			USART1RecvBuf[RecvEnd1] = temp;
			RecvEnd1++;

			if (RecvEnd1 >= USART1MAXBUFFSIZE) RecvEnd1 = 0;
		}

		/* Clear the UART1 Recvive interrupt */
		USART_ClearITPendingBit (USART1, USART_IT_RXNE);
	}
	else if (USART_GetITStatus (USART1, USART_IT_NE | USART_IT_PE | USART_IT_FE | USART_IT_ORE) != RESET)
	{
		USART_GetITStatus (USART1, USART_IT_NE | USART_IT_PE | USART_IT_FE | USART_IT_ORE);
		USART_ReceiveData (USART1);
	}
}


void drv_Usart1SendOne (const u8 dat)
{
	USART_SendData (USART1, dat);

	while (USART_GetFlagStatus (USART1, USART_FLAG_TXE) == RESET) {}

	USART_ClearFlag (USART1, USART_FLAG_TXE);

}



/*****************************************************************************
 �� �� ��  : drv_Usart1SendString
 ��������  : �����ַ���
 �������  : const u8 *Buff  �����ַ���
 �������  : ��
 �� �� ֵ  : 
 
 �޸���ʷ      :
  1.��    ��   : 2019��5��22��
    ��    ��   : �Ŷ�
void drv_Usart1SendString (const u8 *Buff);
    �޸�����   : �����ɺ���

*****************************************************************************/
void drv_Usart1SendString (const u8 *Buff)
{
	while (*Buff != 0)
	{
		drv_Usart1SendOne (*Buff);
		Buff++;
	}    
}


/*****************************************************************************
 �� �� ��  : drv_Usart1SendData
 ��������  : ����ָ�������ַ�
 �������  : const u8 *Buff  �������ݵ�buff
             u16 SendSize  �������ݵĳ���  
 �������  : ��
 �� �� ֵ  : 
 
 �޸���ʷ      :
  1.��    ��   : 2019��5��22��
    ��    ��   : �Ŷ�
void drv_Usart1SendData (const u8 *Buff, u16 SendSize);
    �޸�����   : �����ɺ���

*****************************************************************************/
void drv_Usart1SendData (const u8 *Buff, u16 SendSize)
{
	while (SendSize != 0)
	{
		drv_Usart1SendOne (*Buff);
		Buff++;
		SendSize--;
	}

}



/*****************************************************************************
 �� �� ��  : drv_Usart1RecvReset
 ��������  : ��մ��ڽ��ջ���
 �������  : void  
 �������  : ��
 �� �� ֵ  : 
 
 �޸���ʷ      :
  1.��    ��   : 2019��5��22��
    ��    ��   : �Ŷ�
void drv_Usart1RecvReset (void);
    �޸�����   : �����ɺ���

*****************************************************************************/
void drv_Usart1RecvReset (void)
{
	RecvTop1 = 0;
	RecvEnd1 = 0;

	memset((void *)USART1RecvBuf, 0, USART1MAXBUFFSIZE); 
}



/*****************************************************************************
 �� �� ��  : drv_Usart1RecvOne
 ��������  : ��ȡ1���ֽ�
 �������  : u8 *Str  �������ֽڴ洢��Str[0]
 �������  : ��
 �� �� ֵ  : 
 
 �޸���ʷ      :
  1.��    ��   : 2019��5��22��
    ��    ��   : �Ŷ�
u8 drv_Usart1RecvOne (u8 *Str);
    �޸�����   : �����ɺ���

*****************************************************************************/
u8 drv_Usart1RecvOne (u8 *Str)
{
	if (RecvTop1 == RecvEnd1) return 0;//read nothing

	*Str = USART1RecvBuf[RecvTop1];
	RecvTop1++;

	if (RecvTop1 >= USART1MAXBUFFSIZE) RecvTop1 = 0;

	return 1;//read one

}



/*****************************************************************************
 �� �� ��  : drv_USART1RecvAtTime
 ��������  : ���մ�������
 �������  : u8 *Buff �������ݴ�ŵĵط�                 
             u16 RecvSize  ���ܶ��ٸ��ֽ�            
             u32 timeout_MilliSeconds  ��ʱ�ĺ�����
 �������  : ��
 �� �� ֵ  : ���ؽ��ܵ����ֽ���
 
 �޸���ʷ      :
  1.��    ��   : 2019��5��22��
    ��    ��   : �Ŷ�
u16 drv_USART1RecvAtTime (u8 *Buff, u16 RecvSize, u32 timeout_MilliSeconds);
    �޸�����   : �����ɺ���

*****************************************************************************/
u16 drv_Usart1RecvAtTime (u8 *Buff, u16 RecvSize, u32 timeout_MilliSeconds)
{
//	u16 RecvLen = 0;
//	u8 tmp[1] = {0};

//	if (RecvSize == 0) return 0;

//	g1msTimer5 = timeout_MilliSeconds;

//	while (1)
//	{
//		if (g1msTimer5 == 0) return RecvLen;

//		if (drv_USART1RecvOne (tmp) == 1) 
//		{
//			Buff[RecvLen++] = tmp[0];
//		}

//		if (RecvLen >= RecvSize) return RecvLen;
//	}
	return 0;
}


/*****************************************************************************
 �� �� ��  : dev_Usart1Read
 ��������  : ����������
 �������  : unsigned char *buf  
             int len             
 �������  : ��
 �� �� ֵ  : 
 
 �޸���ʷ      :
  1.��    ��   : 2019��5��22��
    ��    ��   : �Ŷ�
int dev_Usart1Read(unsigned char *buf, int len)
    �޸�����   : �����ɺ���

*****************************************************************************/
u8 drv_Usart1Read(u8 *Buff, u16 len)
{

	u16 RecvLen = 0;
	u8 tmp[1] = {0};

	if (len == 0 || Buff == NULL) return 0;

	while (len--)
	{
		if (drv_Usart1RecvOne (tmp) == 1)
		{
			Buff[RecvLen++] = tmp[0];
		}

		if (RecvLen >= len) return RecvLen;
	}

	return RecvLen;
    
}



  
const char * const g_pcHex = "0123456789abcdef";  
  
/** 
* ���:   һ���򵥵�printf����,֧��\%c, \%d, \%p, \%s, \%u,\%x, and \%X. 
*/  
void UARTprintf(const uint8_t *pcString, ...)  
{  
    uint32_t ulIdx;  
    uint32_t ulValue;       //����Ӳ�����������ջ��ȡ������ֵ�ͱ���  
    uint32_t ulPos, ulCount;  
    uint32_t ulBase;        //������ƻ���,��ʮ������Ϊ10,ʮ����������Ϊ16  
    uint32_t ulNeg;         //Ϊ1��ʾ�ӱ���Ϊ����  
    uint8_t *pcStr;         //����Ӳ�����������ջ��ȡ�����ַ��ͱ���  
    uint8_t pcBuf[32];      //������ֵ�ͱ����ַ�������ַ�  
    uint8_t cFill;          //'%08x'->����8���ַ���'0'���,cFill='0';    
                            //'%8x '->����8���ַ��ÿո����,cFill=' '  
    va_list vaArgP;  
  
    va_start(vaArgP, pcString);  
    while(*pcString)  
    {  
        // ������Ѱ��%���ַ��������ַ�  
        for(ulIdx = 0; (pcString[ulIdx] != '%') && (pcString[ulIdx] != '\0'); ulIdx++)  
        { }  
        
        drv_Usart1SendData(pcString, ulIdx);  
  
        pcString += ulIdx;  
        if(*pcString == '%')  
        {  
            pcString++;  
  
            ulCount = 0;  
            cFill = ' ';  
again:  
            switch(*pcString++)  
            {  
                case '0': case '1': case '2': case '3': case '4':  
                case '5': case '6': case '7': case '8': case '9':  
                {  
                    // �����һ������Ϊ0, ��ʹ��0�����,���ÿո����)  
                    if((pcString[-1] == '0') && (ulCount == 0))  
                    {  
                        cFill = '0';  
                    }  
                    ulCount *= 10;  
                    ulCount += pcString[-1] - '0';  
                    goto again;  
                }  
                case 'c':          
                {  
                    ulValue = va_arg(vaArgP, unsigned long);  
                    drv_Usart1SendData((unsigned char *)&ulValue, 1);  
                    break;  
                }  
                case 'd':     
                {  
                    ulValue = va_arg(vaArgP, unsigned long);  
                    ulPos = 0;  
                      
                    if((long)ulValue < 0)  
                    {  
                        ulValue = -(long)ulValue;  
                        ulNeg = 1;  
                    }  
                    else  
                    {  
                        ulNeg = 0;  
                    }  
                    ulBase = 10;          
                    goto convert;  
                }  
                case 's':  
                {  
                    pcStr = va_arg(vaArgP, unsigned char *);  
  
                    for(ulIdx = 0; pcStr[ulIdx] != '\0'; ulIdx++)  
                    {  
                    }  
                    drv_Usart1SendData(pcStr, ulIdx);  
  
                    if(ulCount > ulIdx)  
                    {  
                        ulCount -= ulIdx;  
                        while(ulCount--)  
                        {  
                            drv_Usart1SendData(" ", 1);  
                        }  
                    }  
                    break;  
                }  
                case 'u':  
                {  
                    ulValue = va_arg(vaArgP, unsigned long);  
                    ulPos = 0;  
                    ulBase = 10;  
                    ulNeg = 0;  
                    goto convert;  
                }  
                case 'x': case 'X': case 'p':  
                {  
                    ulValue = va_arg(vaArgP, unsigned long);  
                    ulPos = 0;  
                    ulBase = 16;  
                    ulNeg = 0;  
         convert:   //����ֵת�����ַ�  
                    for(ulIdx = 1; (((ulIdx * ulBase) <= ulValue) &&(((ulIdx * ulBase) / ulBase) == ulIdx)); ulIdx *= ulBase, ulCount--)       
                    { }  
                    if(ulNeg)  
                    {  
                        ulCount--;                        
                    }  
                    if(ulNeg && (cFill == '0'))  
                    {  
                        pcBuf[ulPos++] = '-';  
                        ulNeg = 0;  
                    }  
                    if((ulCount > 1) && (ulCount < 16))  
                    {  
                        for(ulCount--; ulCount; ulCount--)  
                        {  
                            pcBuf[ulPos++] = cFill;  
                        }  
                    }  
  
                    if(ulNeg)  
                    {  
                        pcBuf[ulPos++] = '-';  
                    }  
  
                    for(; ulIdx; ulIdx /= ulBase)  
                    {  
                        pcBuf[ulPos++] = g_pcHex[(ulValue / ulIdx) % ulBase]; 
                    }  
                    drv_Usart1SendData(pcBuf, ulPos);  
                    break;  
                }  
                case '%':  
                {  
                    drv_Usart1SendData(pcString - 1, 1);                    
                    break;  
                }  
                default:  
                {                      
                    drv_Usart1SendData("ERROR", 5);                    
                    break;  
                }  
            }  
        }  
    }  
    //�ɱ�����������  
    va_end(vaArgP);  
} 




