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
#include "stm32f4xx_conf.h"
#include "drv_usart1.h"

#ifndef IAP_TEST

volatile u8 USART1RecvBuf[USART1MAXBUFFSIZE] = {0};
volatile u16 RecvTop1 = 0;
volatile u16 RecvEnd1 = 0;

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
void drv_Usart1_Init (u32 BaudRate)
{
    //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(USART1_GPIO_CLK,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(USART1_CLK,ENABLE);//ʹ��USART1ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(USART1_GPIO_PORT,USART1_TX_SOURCE,USART1_TX_AF); //GPIOA9����ΪUSART1
	GPIO_PinAFConfig(USART1_GPIO_PORT,USART1_RX_SOURCE,USART1_RX_AF); //GPIOA10����ΪUSART1
	
	//USART1�˿�����
	GPIO_InitStructure.GPIO_Pin = USART1_TX_PIN | USART1_RX_PIN; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(USART1_GPIO_PORT,&GPIO_InitStructure); //��ʼ��PA9��PA10

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


void drv_Usart1_SendOne (const u8 dat)
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
void drv_Usart1_SendString (const u8 *Buff)
{
	while (*Buff != 0)
	{
		drv_Usart1_SendOne (*Buff);
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
void drv_Usart1_SendData (const u8 *Buff, u16 SendSize)
{
	while (SendSize != 0)
	{
		drv_Usart1_SendOne (*Buff);
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
void drv_Usart1_RecvReset (void)
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
u8 drv_Usart1_RecvOne (u8 *Str)
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
u16 drv_Usart1_RecvAtTime (u8 *Buff, u16 RecvSize, u32 timeout_MilliSeconds)
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
u8 drv_Usart1_Read(u8 *Buff, u16 len)
{

	u16 RecvLen = 0;
	u8 tmp[1] = {0};

	if (len == 0 || Buff == NULL) return 0;

	while (len--)
	{
		if (drv_Usart1_RecvOne (tmp) == 1)
		{
			Buff[RecvLen++] = tmp[0];
		}

		if (RecvLen >= len) return RecvLen;
	}

	return RecvLen;
    
}



#if  0  
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
#endif //UARTprintf end

#else

//Private Function Prototype
static uint32_t drv_Usart1_ReadByte(uint8_t *key);

//USART2 get a byte DR
//key: pointer to store data
//return: 
//      0:fail
//      1:success
static uint32_t drv_Usart1_ReadByte(uint8_t *key)
{

  if ( USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
  {
    *key = (uint8_t)USART1->DR;
    return 1;
  }
  else
  {
    return 0;
  }
}


//USART2 get a byte from HyperTerminal
//return: Rx byte
uint8_t drv_Usart1_RecvOne(void)
{
  uint8_t key = 0;

  /* Waiting for user input */
  while (1)
  {
    if (drv_Usart1_ReadByte((uint8_t*)&key)) break;
    
  }
  return key;

}

//Private Function Prototype
//DMA2 for usart1 configuration
static void Usart1_DMA2_Config(void)  
{  
    uint8_t dummy_data = 0;

    DMA_InitTypeDef DMA_InitStructure;
    

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA2ʱ��ʹ�� 
    
    //reset 
    DMA_DeInit(DMA2_Stream7); //���ʹ�ô���1��ֻ�� DMA2 ͨ��4��������7
    
    //DMA configuration  
    DMA_InitStructure.DMA_Channel = DMA_Channel_4; //ͨ��ѡ��
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR; //DMA�����ַ
    DMA_InitStructure.DMA_BufferSize = 1;   //���ݴ���������ʼ��ʱ�ȶ�Ϊ1
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&dummy_data; //DMA �洢��0��ַ     
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral; //�洢��������ģʽ    
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ   
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�洢������ģʽ 
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //�������ݳ���:8λ 
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //�洢�����ݳ���:8λ
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;  // ʹ����ͨģʽ    
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //�е����ȼ�    
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;  //�洢��ͻ�����δ���
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; //����ͻ�����δ���
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold =DMA_FIFOThreshold_Full; 
                
    DMA_Init(DMA2_Stream7, &DMA_InitStructure);    //��ʼ��DMA Stream     
    DMA_Cmd(DMA2_Stream7,ENABLE);                  //����DMA���� 
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE); //ʹ�ܴ���1��DMA����  
}

//usart1 send data via DMA
//@ int8_t *buf: data buffer ready to send
//@ int16_t len: data length
void drv_Usart1_DMA_Send(uint8_t *buf, uint16_t len)
{  
    DMA_InitTypeDef DMA_InitStructure;
    //wait unitl last package is sent
    while(DMA_GetFlagStatus(DMA2_Stream7, DMA_FLAG_TCIF7)==RESET); //�ȴ�DMA������ 
    DMA_ClearFlag(DMA2_Stream7, DMA_FLAG_TCIF7);
    //DMA configuration  
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;   
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR; 
    DMA_InitStructure.DMA_BufferSize = len;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)buf;      
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;     
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;   
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;   
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;     
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;     
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;  
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; 
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold =DMA_FIFOThreshold_HalfFull; 
                
    DMA_Init(DMA2_Stream7, &DMA_InitStructure);         //��ʼ��DMA Stream   
    DMA_Cmd(DMA2_Stream7,ENABLE);                       //����DMA����
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);      //ʹ�ܴ���1��DMA����
}

//usart2 configuration
//default setting is 8,n,1
//@ int baudrate: the desired baudrate
void drv_Usart1_Init(int BaudRate)
{
     //GPIO�˿�����
     GPIO_InitTypeDef GPIO_InitStructure;
     USART_InitTypeDef USART_InitStructure;
//     NVIC_InitTypeDef NVIC_InitStructure;
    
     RCC_AHB1PeriphClockCmd(USART1_GPIO_CLK,ENABLE); //ʹ��GPIOAʱ��
     RCC_APB2PeriphClockCmd(USART1_CLK,ENABLE);//ʹ��USART1ʱ��
    
     //����1��Ӧ���Ÿ���ӳ��
     GPIO_PinAFConfig(USART1_GPIO_PORT,USART1_TX_SOURCE,USART1_TX_AF); //GPIOA9����ΪUSART1
     GPIO_PinAFConfig(USART1_GPIO_PORT,USART1_RX_SOURCE,USART1_RX_AF); //GPIOA10����ΪUSART1
     
     //USART1�˿�����
     GPIO_InitStructure.GPIO_Pin = USART1_TX_PIN | USART1_RX_PIN; //GPIOA9��GPIOA10
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //�ٶ�50MHz
     GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
     GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
     GPIO_Init(USART1_GPIO_PORT,&GPIO_InitStructure); //��ʼ��PA9��PA10
    
    //USART1 ��ʼ������
     USART_InitStructure.USART_BaudRate = BaudRate;//����������
     USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
     USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
     USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
     USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
     USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //�շ�ģʽ
     USART_Init(USART1, &USART_InitStructure); //��ʼ������1       
     USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1   
	 
    //Usart1 NVIC ����
//    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;       //�����ȼ�3
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQͨ��ʹ��
//    NVIC_Init(&NVIC_InitStructure); //����ָ���Ĳ�����ʼ��VIC�Ĵ���    
//    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�    


	//ʹ��USART1 DMA 
	Usart1_DMA2_Config();	
}

#endif





