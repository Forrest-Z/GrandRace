/**************************************************************************
 * @file     fault_injector.c
 * @brief    故障注入
 * @version  V2.0
 * @date     2019-04-25
 * @author   ycy
 ******************************************************************************/
#include "fault_injector.h"
#include	<stdlib.h>
#include "SD_Log_App.h"
#ifdef __TP_USER

#pragma pack(1)

struct list_head m_msg_head;
pListMsgTp m_pmsg;
/*************************************************
Function: 			initFault
Description:		初始化注册故障ID点
Input:					none
Output: 				none
Return: 				none
*************************************************/
void initFault(void)
{
    INIT_LIST_HEAD(&m_msg_head);	//链表初始化
    //注册 初始化
    //CPU 使用率
    createFault(eTP0_reboot);
    createFault(eTP1_CPU_100);
    createFault(eTP2_CPU_90);
    createFault(eTP3_CPU_85);
    createFault(eTP4_SD_FULL);
    createFault(eTP5_bms_ultra_low_temp);
    createFault(eTP6_bms_ultra_high_temp);
    createFault(eTP7_stack_sensor);
    createFault(eTP8_stack_control);
    createFault(eTP9_stack_comm);
	
    createFault(eTP10_stack_fsm);
    createFault(eTP11_stack_log);
		createFault(eTP12_stack_over);
	  createFault(eTP14_wdg_rest);
    createFault(eTP15_comnun_disable);
    createFault(eTP16_tx2_heart_packet_loss);
    createFault(eTP17_bms_low_cap);
    createFault(eTP18_bms_low_temp);
    createFault(eTP19_bms_high_temp);
		
    createFault(eTP20_high_current);
    createFault(eTP21_low_volotage);
    createFault(eTP22_bms_ultralow_cap);
    createFault(eTP23_ultrahigh_current);
    createFault(eTP24_ultralow_volotage);
    createFault(eTP25_out12v_error);
    createFault(eTP26_tx2_volotage_error);
    createFault(eTP27_5v_error);
    createFault(eTP28_in12v_error);
    createFault(eTP29_pad_current_error);
		
    createFault(eTP30_rtk_current_error);
    createFault(eTP31_joy_current_error);
		createFault(eTP32_imu_err_hardware);
		createFault(eTP33_imu_err_drift);
		createFault(eTP34_imu_err_noise);
    createFault(eTP35_driver_right_err_overheat);
    createFault(eTP36_driver_right_err_overvoltage);
    createFault(eTP37_driver_right_err_undervoltage);
    createFault(eTP38_driver_right_err_short_circuit);
    createFault(eTP39_driver_right_err_emergency_stop);
		
    createFault(eTP40_driver_right_err_sensor);
    createFault(eTP41_driver_right_err_sensor);
    createFault(eTP42_driver_right_err_load_config);
    createFault(eTP43_driver_left_err_overheat);
    createFault(eTP44_driver_left_err_overvoltage);
    createFault(eTP45_driver_left_err_undervoltage);
    createFault(eTP46_driver_left_err_short_circuit);
    createFault(eTP47_driver_left_err_emergency_stop);
    createFault(eTP48_driver_left_err_sensor);
    createFault(eTP49_driver_left_err_mos);
		
    createFault(eTP50_driver_left_err_load_config);
    createFault(eTP51_driver_left_CommunFatal);
    createFault(eTP52_driver_right_CommunFatal);
    createFault(eTP53_joy_data_err);
    createFault(eTP54_joy_checksum_err);
    createFault(eTP55_imu_temp_hight_err);
    createFault(eTP56_main2powerCommun_err);
    createFault(eTP57_power2bmsCommun_err);
    createFault(eTP58_ledCommunWarn);
    createFault(eTP59_cpuTempWarn);
    createFault(eTP60_cpuTempErr);
    createFault(eTP61_sdNotConnectErr);
    createFault(eTP62_cmdvelTimeout);
    createFault(eTP63_joyCalibrationErr);
    createFault(eTP64_powerTempWarn);
    createFault(eTP65_powerTempErr);
    //createFault(eTP13_mem_leak);//暂时无法实现
}
/*************************************************
Function: 			create
Description:		注册故障TP ID.
Input:					tp_id desc:描述TP点状态
Output: 				none
Return: 				none
*************************************************/
bool createFault(tp tp_id)
{
    m_pmsg = calloc(1, sizeof(slist_msg_tp));
    if(!m_pmsg)
    {
        return false;
    }

    m_pmsg->id	= tp_id;
    m_pmsg->status	= eTP_DISABLED;
		m_pmsg->cnt	= TP_DEFAULT_CNT;
		
   // memcpy(m_pmsg->desc,desc,strlen(desc));
   // printf_dbg("[new TP:id[%d] status[:%d]\n", m_pmsg->id, m_pmsg->status);

    //插入尾部，list_add()插入首部
    list_add_tail(&m_pmsg->list, &m_msg_head);
    return true;
}

/*************************************************
Function: 			enable
Description:		使能激活该故障ID.
Input:					tp_id
*************************************************/
bool enable(tp tp_id,uint32_t tp_cnt)
{
    return changeStatus(tp_id, eTP_ENABLED,tp_cnt);
}
/*************************************************
Function: 			enable
Description:		禁止该故障ID.
Input:					tp_id
*************************************************/
bool disable(tp tp_id,uint32_t tp_cnt)
{
    return changeStatus(tp_id, eTP_DISABLED,tp_cnt);
}

/*************************************************
Function: 			getstatus
Description:		获取某个点的状态
Input:					tp_id isPrint:是否打印，上传
Output: 				none
Return: 				none
*************************************************/
eTP_status getStatus(tp tp_id,bool isPrint)
{
    bool flag = false ;
    //遍历查询
    list_for_each_entry(m_pmsg, &m_msg_head, list)
    {
        if(m_pmsg->id == tp_id)
        {
            if (isPrint)
            {
                sdlog_printf_app("[main_board get_tp OK id: %d,enable: %d,cnt:%d]",
										m_pmsg->id, m_pmsg->status,m_pmsg->cnt);
            }
            flag = true;
            break;
        }
    }

    if(!flag)
    {
        if (isPrint)
        {
            sdlog_printf_app("main_board tp_id = %d not found!", tp_id);
        }
        return eTP_INVALID;
    }
    return m_pmsg->status;

}
/*************************************************
Function: 			isCanBeExecTp
Description:		获取某个点的状态
Input:					tp_id 
*************************************************/
bool isCanBeExecTp(tp tp_id)
{
    //遍历查询
    list_for_each_entry(m_pmsg, &m_msg_head, list)
    {
        if(m_pmsg->id == tp_id)
        {
            if (eTP_ENABLED == m_pmsg->status
                && m_pmsg->cnt > 0)
            {
                m_pmsg->cnt--;
                return true;
            }
            else
            {
              m_pmsg->status	= eTP_DISABLED;
            }
        }
    }
    return false;
}

/*************************************************
Function: 			getAllStatus
Description:		获取所有点的状态
Input:					tp_id
Output: 				none
Return: 				none
*************************************************/
void getAllStatus(void)
{
    list_for_each_entry(m_pmsg, &m_msg_head, list)
    {
        sdlog_printf_app("[main_board id: %d, status: %d, cnt: %d]", 
          m_pmsg->id, m_pmsg->status,m_pmsg->cnt);
    }
}

/*************************************************
Function: 			changeStatus
Description:		修改某个点的状态
Input:					tp_id,newStatus,tp_cnt
Output: 				none
Return: 				none
*************************************************/
bool changeStatus(tp tp_id, eTP_status newStatus,uint32_t tp_cnt)
{
    bool flag = false ;
		//遍历查询
		list_for_each_entry(m_pmsg, &m_msg_head, list)
		{
				if(m_pmsg->id == tp_id)
				{
						m_pmsg->status	= newStatus;
            m_pmsg->cnt  = tp_cnt;
						sdlog_printf_app("[main_board set OK id: %d,enable: %d,cnt:%d]", 
									m_pmsg->id, m_pmsg->status,m_pmsg->cnt);
						flag = true;
						break;
				}
		}

		if(!flag)
				sdlog_printf_app("main_board tp_id = %d not found!", tp_id);
	
    return flag;
}

#if 0
/*************************************************
Function: 			del_all_list
Description:		删除所有点
Input:					tp_id,newStatus
Output: 				none
Return: 				none
*************************************************/
void del_all_list()
{
    struct list_head m_msg_head, *pos, *n;
    list_for_each_safe(pos, n, &m_msg_head)
    {
        m_pmsg = list_entry(pos, struct msg, list);
        sdlog_printf_app("[main_board delete id: %d, status: %d]", m_pmsg->id, m_pmsg->status);
        list_del(pos);
        free(m_pmsg)
    }
    /*************************************************
    Function: 			del_one_tp
    Description:		删除某个tp点
    Input:					tp_id,newStatus
    Output: 				none
    Return: 				none
    *************************************************/
    void del_one_tp(tp tp_id)
    {
        struct list_head m_msg_head, *pos, *n;
        bool flag = false ;

        list_for_each_safe(pos, n, &m_msg_head)
        {
            m_pmsg = list_entry(pos, struct msg, list);
            if(m_pmsg->msgid == tp_id)
            {
                sdlog_printf_app("[main_board delete id: %d, status: %d]", m_pmsg->id, m_pmsg->status);
                list_del(pos);
                free(m_pmsg);
                flag = true;
            }
        }
        if(!flag)
            sdlog_printf_app("main_board id : %d not found!\n", key);

    }
#endif



#endif
