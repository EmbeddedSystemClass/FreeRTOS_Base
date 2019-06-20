/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : comm.c
  �� �� ��   : ����
  ��    ��   : �Ŷ�
  ��������   : 2019��6��18��
  ����޸�   :
  ��������   : ��������ָ��
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2019��6��18��
    ��    ��   : �Ŷ�
    �޸�����   : �����ļ�

******************************************************************************/

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include "comm.h"
#include "drv_usart1.h"

/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
#define MAX_RXD_BUF_LEN        			100
#define MAX_TXD_BUF_LEN					100      
        
/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/
typedef struct
{
    volatile uint8_t RxdStatus;                 //����״̬
    volatile uint8_t RxCRC;                     //Ч��ֵ
    volatile uint8_t RxdFrameStatus;            //���հ�״̬
    volatile uint8_t NowLen;                    //�����ֽ�ָ��
    volatile uint8_t RxdTotalLen;               //���հ����ݳ���
    volatile uint8_t RxdBuf[MAX_RXD_BUF_LEN];   //���հ����ݻ���    
}COMMAND_T;

static COMMAND_T gcmd;


/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/

    
void init_serial_boot(void)
{
    gcmd.RxdStatus = 0;
    gcmd.RxCRC = 0;
    gcmd.RxdFrameStatus = SPACE;
}

/*****************************************************************************
 �� �� ��  : DealSerialParse
 ��������  :     ���յ������ݽ��������ݰ���
 �������  : void
 �������  : ��
 �� �� ֵ  : void
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2019��6��18��
    ��    ��   : �Ŷ�
    �޸�����   : �����ɺ���

*****************************************************************************/
// 7E 01 A1 00 AA 74 
// 7E 01 A1 AA 25 48 00 AA 74 7E 01 A1 00 AA 74

static void dbh(int index,char *buf,int len)
{
    int i = 0;
    printf("%d. -----------------\r\n",index);
    for(i=0;i<len;i++)
    {
        printf("%02x ",buf[i]);
    }

    printf("\r\nbuf length = %d ,print end \r\n",len);    
}


void DealSerialParse(void)
{
    uint8_t ch = 0;
    while(1)
    {   
        if(drv_Usart1_RecvOne(&ch) != 1)  //��ȡ��������
        {
            return;
        }   
        
        switch (gcmd.RxdStatus)
        { /*��������״̬*/                
            case 0:
                if(FH == ch) /*���հ�ͷ*/
                {
                    gcmd.RxdBuf[0] = ch;
                    gcmd.RxCRC = ch;    
                    gcmd.NowLen = 1;
                    gcmd.RxdStatus = 20;
                }
                break;              
            case 20:      /* �����������ݰ� */
                if (ch == FT) 
                {
                    gcmd.RxdStatus = 21;
                }
                gcmd.RxdBuf[gcmd.NowLen++] = ch;
                gcmd.RxCRC ^= ch;
//                dbh(3,gcmd.RxdBuf,gcmd.NowLen);
                break;
            case 21:
                if(ch == gcmd.RxCRC)
                {    
                    gcmd.RxdBuf[gcmd.NowLen++] = ch;
                    gcmd.RxdTotalLen = gcmd.NowLen;
                    gcmd.RxdFrameStatus = FINISH;                    
                    gcmd.RxdStatus = 0;

                    printf("Deal Serial Parse finish\r\n");
//                    dbh(1,gcmd.RxdBuf,gcmd.NowLen);
                    break;
                }
                else
                {  
                    gcmd.RxdFrameStatus = FINISH;
                    gcmd.RxdBuf[gcmd.NowLen++] = ch;
                    gcmd.RxCRC ^= ch;
                    gcmd.RxdStatus = 20;
//                    dbh(2,gcmd.RxdBuf,gcmd.NowLen);
                }
				
                break;
            default:                
                if (gcmd.RxdFrameStatus == SPACE) 
                {
                    gcmd.RxdFrameStatus = FINISH;
                    gcmd.RxdStatus = 0;
                }
            break;
         }
    }
}


#if 1
/*****************************************************************************
 �� �� ��  : DealRxData
 ��������  : �����յ����������ݰ�������Ӧ���ݰ���
 �������  : void
 �������  : ��
 �� �� ֵ  : void
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2019��6��18��
    ��    ��   : �Ŷ�
    �޸�����   : �����ɺ���

*****************************************************************************/
void DealRxData(void)
{
    uint16_t ch;
    uint8_t bcc = 0;       
    
    if (FINISH == gcmd.RxdFrameStatus)
    {
//        printf("\r\n -------------------\r\n");
//        dbh(4,gcmd.RxdBuf,gcmd.RxdTotalLen);
        
        if(gcmd.RxdBuf[0] == FH)                                   //����02���ݰ�
        {
            for (ch = 0; ch < gcmd.RxdTotalLen - 1; ch++)          //��������
            {
                    bcc ^= gcmd.RxdBuf[ch];
            }
            
            if (bcc == gcmd.RxdBuf[gcmd.RxdTotalLen -1])
            {

                switch (gcmd.RxdBuf[2])
                {
                    case GETSENSOR:                        
                        printf("recv cmd = A1 \r\n");
                        //SendData(gcmd.RxdBuf[2]);
                        //init_serial_boot();
                        SystemReset();
                        break;
                    case SETLED:
                        printf("recv cmd = A2 \r\n");
                        SendData(gcmd.RxdBuf[2]);
                        init_serial_boot();
                        break;                        
                    case GETDEVICESAT:
                        printf("recv cmd = A3 \r\n");
                        SendData(gcmd.RxdBuf[2]);
                        init_serial_boot();
                        break;
                    case GETVER:
                        printf("recv cmd = A4 \r\n");
                        SendData(gcmd.RxdBuf[2]);
                        init_serial_boot();
                        break; 
                    case HEARTBEAT:
                        printf("recv cmd = A6 \r\n");
                        SendData(gcmd.RxdBuf[2]);
                        init_serial_boot();
                        break;
                    case UPGRADE:
                        printf("recv cmd = A7 \r\n");
                        SendData(gcmd.RxdBuf[2]);
                        init_serial_boot();
                        break;                         
                    
                }
            }
            else
            {
                init_serial_boot();
            }
        }
        else
        {
            init_serial_boot();
        }
    }

}

#endif







//void DealSerialParse(void)
//{
//    uint8_t ch = 0;

//        if(drv_Usart1_RecvOne(&ch) != 1)  //��ȡ��������
//        {
//            return;
//        }   
//        

//	if(FH == ch)
//	{
//	    gcmd.RxdBuf[0] = ch;
//	    gcmd.RxCRC = ch;    
//	    gcmd.NowLen = 1;
//	    drv_Usart1_Read(gcmd.RxdBuf+1,����);
//            //�ж�����
//            //�������������CRC
//            //���ǳ����⣬��������
//	}
//}

//��������̸о��е㱿׾

static uint8_t calcCRC(uint8_t *buf,uint8_t len)
{
    uint8_t i = 0;
    uint8_t bcc = 0;
    bcc = buf[0];

    printf("len = %d\r\n",len);

    for(i=1;i<len;i++)
    {
        bcc ^= buf[i];
    }

    return bcc;
}



void SendData(uint8_t frame_type)
{
    uint8_t i = 0;
    uint8_t TxdBuf[MAX_TXD_BUF_LEN]={0};

    switch (frame_type)
    {
        case GETSENSOR:
            i = 3;
            memcpy(TxdBuf,(uint8_t *)gcmd.RxdBuf,i);
            TxdBuf[i++] = 0x02;
            TxdBuf[i++] = CMDSUCC;
            TxdBuf[i++] = 0x01;
            TxdBuf[i++] = FT;
            TxdBuf[i++] = calcCRC(TxdBuf,i);
            break;
        case SETLED:
            i = 3;
            memcpy(TxdBuf,(uint8_t *)gcmd.RxdBuf,i);
            TxdBuf[i++] = 0x01;
            TxdBuf[i++] = CMDSUCC;
            TxdBuf[i++] = FT;
            TxdBuf[i++] = calcCRC(TxdBuf,i);
            break;                        
        case GETDEVICESAT:
            i = 3;
            memcpy(TxdBuf,(uint8_t *)gcmd.RxdBuf,i);
            TxdBuf[i++] = 0x02;
            TxdBuf[i++] = CMDSUCC;
            TxdBuf[i++] = 0x01;
            TxdBuf[i++] = FT;
            TxdBuf[i++] = calcCRC(TxdBuf,i);
            break;
        case GETVER:
            i = 3;
            memcpy(TxdBuf,(uint8_t *)gcmd.RxdBuf,i);
            TxdBuf[i++] = 0x02;
            TxdBuf[i++] = CMDSUCC;
            TxdBuf[i++] = 0x01;
            TxdBuf[i++] = FT;
            TxdBuf[i++] = calcCRC(TxdBuf,i);
            break; 
        case HEARTBEAT:
            i = 3;
            memcpy(TxdBuf,(uint8_t *)gcmd.RxdBuf,i);
            TxdBuf[i++] = 0x02;
            TxdBuf[i++] = CMDSUCC;
            TxdBuf[i++] = 0x01;
            TxdBuf[i++] = FT;
            TxdBuf[i++] = calcCRC(TxdBuf,i);
            break;
        case UPGRADE:
            i = 3;
            memcpy(TxdBuf,(uint8_t *)gcmd.RxdBuf,i);
            TxdBuf[i++] = 0x02;
            TxdBuf[i++] = CMDSUCC;
            TxdBuf[i++] = 0x01;
            TxdBuf[i++] = FT;
            TxdBuf[i++] = calcCRC(TxdBuf,i);
            break;                         

        default:
            init_serial_boot(); 
            return;
    }

    dbh(100,(char *)TxdBuf,i);
    drv_Usart1_SendData(TxdBuf,i);
}

