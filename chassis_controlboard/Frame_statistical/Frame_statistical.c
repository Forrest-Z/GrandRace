#include "Frame_statistical.h"
#include "protocol.h"
#include "OD.h"
#include "string.h"
unsigned char CONTOTX2_INFO_ID;
void MainBoard_SendFrame_Statistical(unsigned char *data)
{
    CONTOTX2_INFO_ID = *(data+8);
    statistics_contotx2_msg.frame_total++;
    switch(CONTOTX2_INFO_ID)
    {
    case BATTERY_INFO_ID:
        statistics_contotx2_msg.battery_state++;
        break;
    case MONITOR_INFO_ID:
        statistics_contotx2_msg.monitor_info++;
        break;
    case ULTRASONIC_INFO_ID:
        statistics_contotx2_msg.ultansound++;
        break;
    case LOG_UP_ID:
        statistics_contotx2_msg.log_up++;
        break;
    case 0:
        statistics_contotx2_msg.heart_singnal++;
        break;
    case STATE_SWITCH_UP_ID:
        statistics_contotx2_msg.state_change++;
        break;
    case IMU_INFO_ID:
        statistics_contotx2_msg.imu++;
        break;
    case ODOM_INFO_ID:
        statistics_contotx2_msg.odom++;
        break;
    case EVENT_UP_ID:
        statistics_contotx2_msg.event_up++;
        break;
    case CONTROL_DEBUG_ID:
        statistics_contotx2_msg.control_up++;
        break;
    case CHASSIS_SPEED_ID:
        statistics_contotx2_msg.chassis_speed++;
        break;
    case CHASSIS_MILEAGE_ID:
        statistics_contotx2_msg.chassis_odom++;
        break;
    case TX2_POWEROFF_ID:
        statistics_contotx2_msg.poweroff++;
        break;
    case ULTRASONIC_UP_ID:
        statistics_contotx2_msg.ultansound_up++;
        break;
		case ALARM_PUB_ID:
			statistics_contotx2_msg.chassis_alarm++;
		break;
		case CONTROL_INFO_ID:
			statistics_contotx2_msg.control_info++;
		break;
    default:
        statistics_contotx2_msg.frame_total--;
        break;
    }
}
void MainBoard_RecivceFrame_Statistical(unsigned char INFO_ID)
{
    statistics_tx2tocon_msg.frame_total++;
    switch(INFO_ID)
    {
    case STATE_SWITCH_DOWN_ID:
        statistics_tx2tocon_msg.control_state++;
        break;
    case LOG_DOWN_ID:
        statistics_tx2tocon_msg.cmd_req++;
        break;
    case CMD_VEL_ID:
        statistics_tx2tocon_msg.cmd_vel++;
        break;
    case SPEED_LIMIT:
        statistics_tx2tocon_msg.speed_limit++;
        break;
    case TX2_POWEROFF_ACK_ID:
        statistics_tx2tocon_msg.poweroff_res++;
        break;
    case ULTRASONIC_DOWN_ID:
        statistics_tx2tocon_msg.ultrasonic_down++;
        break;
    case CONTROL_PARA_ID:
        statistics_tx2tocon_msg.control_para++;
        break;
    }
}

STATISTICS_CONTOTX2_MSG *contotx2_buf = NULL;
STATISTICS_TX2toCON_msg *tx2tocon_buf = NULL;
void Cache_contotx2_framebuffer(STATISTICS_CONTOTX2_MSG *data)
{
    static STATISTICS_CONTOTX2_MSG buf;
    if(data == NULL)
    {
        return;
    }

    //contotx2_buf = malloc(sizeof(STATISTICS_CONTOTX2_MSG));
    contotx2_buf = &buf;
    if (NULL == contotx2_buf)
    {
        return;
    }

    memcpy(contotx2_buf,data,sizeof(STATISTICS_CONTOTX2_MSG));
}
void Cache_tx2tocon_framebuffer(STATISTICS_TX2toCON_msg *data)
{
    static STATISTICS_TX2toCON_msg buf;
    if(data == NULL)
    {
        return;
    }
    tx2tocon_buf = &buf;
    //tx2tocon_buf = malloc(sizeof(STATISTICS_TX2toCON_msg));
    if (NULL == tx2tocon_buf)
    {
        return;
    }

    memcpy(tx2tocon_buf,data,sizeof(STATISTICS_TX2toCON_msg));
}
void Free_contotx2_buffer(void)
{
    //free(contotx2_buf);
    contotx2_buf = NULL;
}
void Free_tx2tocon_buffer(void)
{
    //free(tx2tocon_buf);
    tx2tocon_buf = NULL;
}
