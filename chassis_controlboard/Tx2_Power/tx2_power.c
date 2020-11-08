#include "Clock.h"
#include "tx2_power.h"
#include "Publish_Pc.h"
#include "info_core.h"
#include "Subscribe_Pc.h"
#include "Publish_powerboard.h"
#include "SD_Log_App.h"


void openTx2Power(void)
{
	//检检测tx2引脚是否开机 
	if(HAL_GPIO_ReadPin(GPIOG,GPIO_PIN_6)==GPIO_PIN_SET)
	{
		HAL_GPIO_WritePin(TX2_POWER_GPIO_Port, TX2_POWER_Pin, GPIO_PIN_RESET);
		delayUs(300000);	
		HAL_GPIO_WritePin(TX2_POWER_GPIO_Port, TX2_POWER_Pin, GPIO_PIN_SET);
		delayUs(200000);		
	}

  //再次检测
  delayUs(1000000);//1s
	if(HAL_GPIO_ReadPin(GPIOG,GPIO_PIN_6)==GPIO_PIN_SET)
	{
    delayUs(2000000);//2s
    delayUs(2000000);//2s
    delayUs(1000000);//1s
    
		HAL_GPIO_WritePin(TX2_POWER_GPIO_Port, TX2_POWER_Pin, GPIO_PIN_RESET);
		delayUs(300000);	
		HAL_GPIO_WritePin(TX2_POWER_GPIO_Port, TX2_POWER_Pin, GPIO_PIN_SET);
		delayUs(200000);	
	}

}

static uint8_t poweroff_state = POWEROFF_STATE_INIT;

void SetPoweroffState(uint8_t state)
{
    poweroff_state = state;
}

uint8_t GetPoweroffState(void)
{
    return poweroff_state;
}

void handlerTx2PowerOff(void)
{
    static uint32_t poweroff_delay = 0x00;
    static uint32_t close_delay = 0x00;

    switch(GetPoweroffState())
    {
    case POWEROFF_STATE_INIT:
//			debugPrint(82);
		  //  GR_LOG_INFO("--Tx2 power off state init--");
        break;

    case POWEROFF_STATE_BUTTON:
        SetPoweroffState(POWEROFF_STATE_REQUEST);
        //SetPoweroffState(POWEROFF_STATE_CLOSE);
        Tx2PoweroffCleanAck();
        Tx2PoweroffRequestPubHandle();
        poweroff_delay = getSystemTick();
		    GR_LOG_WARN("--Tx2 power off state button--");

        break;

    case POWEROFF_STATE_REQUEST:
        if((getSystemTick() < poweroff_delay + 5000))
        {
            if((getTX2CommunicationState() == 0) && Tx2PoweroffGetAck())
            {
                close_delay = getSystemTick();
                Tx2PoweroffCleanAck();
                SetPoweroffState(POWEROFF_STATE_CLOSE);
                GR_LOG_WARN("--Tx2 power off state close--");
            }
        }
        else
        {
            Tx2PoweroffCleanAck();
            SetPoweroffState(POWEROFF_STATE_INIT);
            GR_LOG_WARN("--Tx2 power off state init--");
        }

        break;

    case POWEROFF_STATE_CLOSE:
        if((getSystemTick() > close_delay + 8000))
        {
						Reboot_Power(all_power,ON);
            SetPoweroffState(POWEROFF_STATE_INIT);
            GR_LOG_INFO("--Tx2 power off state init--");
        }
        break;

    default:
        break;
    }
}

