/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : ini.h
  �� �� ��   : ����
  ��    ��   : �Ŷ�
  ��������   : 2019��5��28��
  ����޸�   :
  ��������   : �Բ������в���
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2019��5��28��
    ��    ��   : �Ŷ�
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef __INI_H
#define __INI_H
#include "easyflash.h"
#include "sfud.h"
#include "stdio.h"


#define SF_VERSION "V1.0.0.1"
#define HD_VERSION "V1.0"

const ef_env default_param_value[] = {
    {"iap_need_copy_app","0"},
    {"iap_copy_app_size","0"},
    {"stop_in_bootloader","0"},
    {"device_id","1"},
    {"boot_times","0"},
//����Ϊbase��ͨ�ò���    
    {"HardWare Version",HD_VERSION},
    {"SoftWare Version",SF_VERSION}

};
    


EfErrCode RestoreDefaultSetting(ef_env const ** defaultValue, size_t *defaultSize);





#endif
