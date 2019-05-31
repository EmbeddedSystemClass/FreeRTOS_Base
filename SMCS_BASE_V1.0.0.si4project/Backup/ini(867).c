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



const ef_env default_param_value[] = {
    {"iap_need_copy_app","0"},
    {"iap_copy_app_size","0"},
    {"stop_in_bootloader","0"},
    {"device_id","1"},
    {"boot_times","0"},
//����Ϊbase��ͨ�ò���    
    {"HardWare Version",HD_VERSION},
    {"SoftWare Version",SF_VERSION},

};



/*****************************************************************************
 �� �� ��  : RestoreDefaultSetting
 ��������  : �ָ�Ĭ������
 �������  : defaultValue
             defaultSize
 �������  : ��
 �� �� ֵ  : void
 
 �޸���ʷ      :
  1.��    ��   : 2019��5��28��
    ��    ��   : �Ŷ�

    �޸�����   : �����ɺ���

*****************************************************************************/
void RestoreDefaultSetting(void)
{
    ef_env const **default_env;
    size_t *default_env_size;
    
    *default_env = default_param_value;
    *default_env_size = sizeof(default_param_value) / sizeof(default_param_value[0]);


    ef_env_init(default_env,default_env_size);

    ef_env_set_default();
}
