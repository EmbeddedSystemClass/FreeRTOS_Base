/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : ini.c
  �� �� ��   : ����
  ��    ��   : �Ŷ�
  ��������   : 2019��5��28��
  ����޸�   :
  ��������   : ��������
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2019��5��28��
    ��    ��   : �Ŷ�
    �޸�����   : �����ļ�

******************************************************************************/

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include "ini.h"

//static const ef_env default_param[] = 
//{
//   {"boot_times","0"},
//   {"device_id","1"},
//   {"software version","v1.0.0.0"},
//   {"hardware version","v1.0"},
//   {"key1","key1_value"},
//   {"key2","key2_value"},
//   {"key3","key3_value"},
//   {"key4","key4_value"},
//   {"key5","key5_value"},
//   {"key6","key6_value"},
//   {"key7","key7_value"},
//   {"key8","key8_value"}, 
//};


//#define param_start_addr 0x00

/*****************************************************************************
 �� �� ��  : RestoreDefaultSetting
 ��������  : �ָ�Ĭ������,ʵ����д��ϵͳ��������Ҫ�Ĳ���
           ��ʹ��ef_env_set_default ����Ҫ��ef_port.c�ж�Ĭ��ֵ�����޸�
 �������  : 
             
 �������  : ��
 �� �� ֵ  : void
 
 �޸���ʷ      :
  1.��    ��   : 2019��5��28��
    ��    ��   : �Ŷ�

    �޸�����   : �����ɺ���

*****************************************************************************/
void RestoreDefaultSetting(void)
{
//    size_t i = 0;
//    size_t default_param_size = sizeof(default_param) / sizeof(default_param[0]);
//    size_t add = EF_START_ADDR;
//    
//    //����FLASH
//    for(add=EF_START_ADDR;add<ENV_AREA_SIZE+EF_START_ADDR;add+=EF_ERASE_MIN_SIZE)
//    {
//        if(ef_port_erase(EF_START_ADDR, EF_ERASE_MIN_SIZE) != EF_NO_ERR)
//        {
//            printf("ef_port_erase error\r\n");
//            return;
//        }        
//    }

//    printf("ef_port_erase success\r\n");

//    //��дĬ�ϲ���
//    for(i= 0;i<default_param_size;i++)
//    {
//        if(ef_set_env(default_param[i].key, default_param[i].value) != EF_NO_ERR)
//        {
//            printf("i = %d ef_port_erase error\r\n",i);
//            return;
//        }

//        printf("key = %s, value = %s\r\n",default_param[i].key, default_param[i].value);
//    }  

//    printf("RestoreDefaultSetting success\r\n");

    if(ef_env_set_default()== EF_NO_ERR)
    {
        printf("RestoreDefaultSetting success\r\n");
    }
    
}

