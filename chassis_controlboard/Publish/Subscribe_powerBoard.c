#include "Subscribe_powerBoard.h"
#include "OD.h"
#include <string.h>
#include "tx2_power.h"
#include "SD_Log_App.h"
#include "log_app.h"
//#include "monitor.h"

void bmsProtocolCallBack(void)
{
  	// 开机8s内不记录下面的故障(系统稳定后记录)
    if(getSystemTick() < 8000)	
    {      
       return;
    }
    checkBmsData();
}

void updateBmsProtocolData(Bms_Buffer *p)
{
    memcpy(p, &Bms_Info, sizeof(Bms_Buffer));
}

void PowerRequestCallBack(void)
{
//	Reboot_Power(all_power,ON);
	static uint8_t poweroff_logflag = 0;
    if(Power_Request_Buffer.ID_Code == 0x05)
    {
			  if(poweroff_logflag == 0)
				{
					GR_LOG_WARN("get poweroff cmd 0x05!");  
					poweroff_logflag = 1;
				}
        if(POWEROFF_STATE_INIT == GetPoweroffState())
        {
            SetPoweroffState(POWEROFF_STATE_BUTTON);
        }
    }
//	else if(Power_Request_Buffer.ID_Code == 0x01)
//	{
//		updateMotorStopData(Power_Request_Buffer.Function_Code);
//	}
}
void Power_VersionCallback(void)
{
		if (!g_bVersion)
					sdlog_printf_app("%s" ,Power_version);
		GR_LOG_INFO("%s" ,Power_version);
}
