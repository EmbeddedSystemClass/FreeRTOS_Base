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

static const ef_env default_param[] = 
{
   {"boot_times","0"},
   {"device_id","1"},
   {"software version","v1.0.0.0"},
   {"hardware version","v1.0"},
   {"key1","key1_value"},
   {"key2","key2_value"},
   {"key3","key3_value"},
   {"key4","key4_value"},
   {"key5","key5_value"},
   {"key6","key6_value"},
   {"key7","key7_value"},
   {"key8","key8_value"}, 
};


#define param_start_addr 0x00

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
EfErrCode RestoreDefaultSetting(void)
{
    EfErrCode result = EF_NO_ERR;
    uint32_t addr, i, value_len;
    struct sector_meta_data sector;

    size_t default_param_size = sizeof(default_param) / sizeof(default_param[0]);
    
    EF_ASSERT(default_param);
    EF_ASSERT(default_param_size);

    /* lock the ENV cache */
    ef_port_env_lock();
    /* format all sectors */
    for (addr = param_start_addr; addr < param_start_addr + ENV_AREA_SIZE; addr += SECTOR_SIZE) {
        result = format_sector(addr, SECTOR_NOT_COMBINED);
        if (result != EF_NO_ERR) {
            goto __exit;
        }
    }
    /* create default ENV */
    for (i = 0; i < default_param_size; i++) {
        /* It seems to be a string when value length is 0.
         * This mechanism is for compatibility with older versions (less then V4.0). */
        if (default_param[i].value_len == 0) {
            value_len = strlen(default_param[i].value);
        } else {
            value_len = default_param[i].value_len;
        }
        sector.empty_env = FAILED_ADDR;
        create_env_blob(&sector, default_param[i].key, default_param[i].value, value_len);
        if (result != EF_NO_ERR) {
            goto __exit;
        }
    }

__exit:
    /* unlock the ENV cache */
    ef_port_env_unlock();

    return result;    
}

