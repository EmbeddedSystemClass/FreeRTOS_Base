/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : comm.h
  �� �� ��   : ����
  ��    ��   : �Ŷ�
  ��������   : 2019��6��18��
  ����޸�   :
  ��������   : ����ͨѶЭ�����������
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2019��6��18��
    ��    ��   : �Ŷ�
    �޸�����   : �����ļ�

******************************************************************************/
#ifndef __COMM_H
#define __COMM_H

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
#define FH		    					0x7E	/* frame head */
#define FT                              0xAA    /* frame tail */
#define CMDSUCC	    					0x06	/* command success */
#define CMDERRO	    					0x15	/* command error */
#define UPDAT							0x13	/* Upload data */
    
    
#define SPACE		        			0x00
#define FINISH		       	 			0x55

#define GETSENSOR                       0xA1
#define SETLED                          0xA2
#define GETDEVICESAT                    0xA3
#define GETVER                          0xA4
#define UPSENSOR                        0xA5
#define HEARTBEAT                       0xA6
#define UPGRADE                         0xA7


/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/
void DealRxData(void);
void DealSerialParse(void);
void init_serial_boot(void);

void SendData(unsigned char frame_type);


#endif

