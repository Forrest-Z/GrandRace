#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#pragma pack(1)
//====================================================================================================================//

typedef enum {
    Power_Control_ID = 5,
    power_down_send_ID =6,
    PowerVer_ID = 7,
    STATE_SWITCH_UP_ID  = 25,
    MONITOR_INFO_ID = 27,				// 27
    IMU_INFO_ID = 29,
    ODOM_INFO_ID = 30,
    JOINT_INFO_ID = 31,
    TF_INFO_ID = 32,
    ULTRASONIC_INFO_ID = 33,
    BATTERY_INFO_ID = 34,
    LOG_UP_ID = 35,
    EVENT_UP_ID = 36,
    CONTROL_DEBUG_ID = 37,
    CHASSIS_SPEED_ID = 38,   	// 38
    CHASSIS_MILEAGE_ID = 39,   // 39
    TX2_POWEROFF_ID = 40,			// 40
    ULTRASONIC_UP_ID = 41,			// 41
    CON_UP_SENDTOALE_ID = 42,  // 42
    CON_UP_RECIVTOALE_ID = 43, // 43
    ALARM_PUB_ID = 44,      		// 44
		CONTROL_INFO_ID = 45,
		PUB_COM_DELAYPUB_ID = 46,
    MainBoard_Online_Update_ID = 50,

    joy_down_send_ID = 75,//摇杆区域
    JoyVer_ID = 76,
    PUB_JOY_RESET_ID = 77, //摇杆标定接口

    //ult 超声波
    PUB_ULT_SAMPLE_SEND_ID = 80,
    PUB_ULT_UPDATE_ID	= 81,
    PUB_UltVer_ID = 82,
    PUB_Ult_REBOOT_ID = 83,
    PUB_Ult_SET_CHA_ID = 84,//ult 通道 Channel
    PUB_Ult_GET_CHA_ID = 85,//ult 通道 Channel
    Led_down_send_ID = 100, //灯带区域
    PUB_LED_VER_ID = 101,
    PUB_LED_CTRL_ID =102, //Led 控制 ID
} PublishID;

typedef enum {
    Bms_Info_ID = 5,
    Power_Request_ID = 6,
    SUB_Power_ok_ID = 7,
    PowerBoard_Online_ID = 8,
    Power_version_ID = 9,
    STATE_SWITCH_DOWN_ID  = 25,
    CMD_VEL_ID = 26,						// 26
    LOG_DOWN_ID = 27,					// 27
    CONTROL_PARA_ID = 28,			// 28
    SPEED_LIMIT = 29,					// 29
    TX2_POWEROFF_ACK_ID = 30, 	// 30
    ULTRASONIC_DOWN_ID = 31,		// 31
    CON_SNEDFRAME_ID = 32,     // 32
    CON_RECIVCEFRAME_ID = 33,  // 33
    //ALARM_SUB_ID = 34,      		// 34
    COM_DELAY_ID = 34,//用于接收TX2下发的统计通信延时信号
		SensorBoard_Online_Update_ID = 75,

    SUB_JOY_DATA_ID    = 76,	//摇杆区域 Joystick_Data_ID
    SUB_JOY_VERSION_ID = 77,
    SUB_JOY_RESET_ID   = 78,  //摇杆标定接口
    
    //ult 超声波
    SUB_ULT_SAMPLE_RECV_ID = 80,
    SUB_ULT_UPDATE_ID	= 81,
    SUB_UltVer_ID		 =	82,
    SUB_Ult_GET_CHA_ID = 85,    //获取ult 通道

    LedBoard_Online_Update_ID = 100,//灯带区域
    SUB_LED_VERSION_ID = 101,
} SubscriberID;

//====================================================================================================================//

typedef struct _TIME_STAMP
{
    unsigned long int seq;
    unsigned long int sec;
    unsigned long int nsec;
} Time_StampTypeDef;

typedef struct _BATTERY_INFO
{
    Time_StampTypeDef time;
    unsigned long int voltage;        //电池电压
    float           	current;        //电池电流
    signed long int   temperature;    //电池温度
    unsigned long int charge;         //充电状态
    unsigned long int capacity;       //电池当前容量
    unsigned long int design_capacity;//电池设计总容量
    unsigned long int percentage;     //当前电量百分比
} Battery_InfoTypeDef;


typedef struct _MONITOR_INFO
{
    Time_StampTypeDef time;
    unsigned short int 	stack_sensor;
    unsigned short int 	stack_control;
    unsigned short int 	stack_comm;
    unsigned short int 	stack_fsm;
    unsigned short int 	stack_log;
    unsigned short int  stack_res[3];
    unsigned char	    	cpu_usage;
    unsigned char   		wdg_state;
    unsigned long  int  bms;
    unsigned short int	motor;
    unsigned short int  motor_driver;
    unsigned char 			joystick;
    unsigned char 			imu;
    unsigned short int  ultrasonic;
    unsigned char 			weight;
    unsigned char 			flash;
    unsigned char		    other_res[10];
} Monitor_InfoTypeDef;

typedef struct _STATE_EVENT_UP
{
    Time_StampTypeDef time;
    unsigned char	    state;
} State_Event_UpTypeDef;

typedef struct _STATE_SWITCH_UP
{
    Time_StampTypeDef time;
    unsigned char	    state;
} State_Switch_UpTypeDef;

typedef struct _ULTRASONIC_INFO
{
    Time_StampTypeDef time;
    unsigned short int count;
    unsigned short int distance[14];
} Ultrasonic_InfoTypeDef;

typedef struct _ULTRASONIC_COMM
{
    Time_StampTypeDef time;
    unsigned short int len;
    unsigned char info[128];
} Ultrasonic_CommTypeDef;

typedef struct _ULTRASONIC_SAMPLE
{
    unsigned short int len;
    unsigned char info[128];
} Ultrasonic_SampleTypeDef;

typedef struct _IMU_INFO
{
    Time_StampTypeDef time;
    float							linear_x;
    float							linear_y;
    float							linear_z;
    float							angular_x;
    float							angular_y;
    float							angular_z;
    float							orientation_x;
    float							orientation_y;
    float							orientation_z;
    float							orientation_w;
} Imu_InfoTypeDef;

typedef struct _ODOM_INFO
{
    Time_StampTypeDef time;
    float							position_x;
    float							position_y;
    float							position_z;
    float							linear_x;
    float							linear_y;
    float							linear_z;
    float							angular_x;
    float							angular_y;
    float							angular_z;
    float							orientation_x;
    float							orientation_y;
    float							orientation_z;
    float							orientation_w;
} Odom_InfoTypeDef;

typedef struct CONTROL_INFO
{
	Time_StampTypeDef time;	//时间戳
	double frequncy;				//频率
	double linear_vel;			//线速度
	double angular_del;			//角速度
}Control_InfoTypeDef;
typedef struct _LOG_UP
{
    Time_StampTypeDef time;
    unsigned char     len;
    char              info[128];
} Log_UpTypeDef;

//====================================================================================================================//

typedef struct _LOG_DOWN
{
    unsigned char     len;
    char              cmd[32];
} Log_DownTypeDef;

typedef struct _LOG_TP
{
    unsigned char     len;
    char              cmd[64];
} Log_TPTypeDef;


typedef struct _STATE_SWITCH_DOWN
{
    unsigned char	    state;
} State_Switch_DownTypeDef;

typedef struct _CMD_VEL
{
    float							linear_x;
    float							linear_y;
    float							linear_z;
    float							angular_x;
    float							angular_y;
    float							angular_z;
} Cmd_VelTypeDef;

typedef struct _CONTROL_VEL
{
    float							goal_v_speed;
    float							goal_a_speed;
    float							goal_v_speed_pre;
    float							goal_a_speed_pre;
    float							present_v_speed;
    float							present_a_speed;
    float							present_left_speed;
    float							present_right_speed;
    float 						goal_left_speed;
    float 						goal_right_speed;
    float							present_left_pwm;
    float							present_right_pwm;
		float							present_left_elec;
		float							present_right_elec;
} Control_VelTypeDef;

typedef struct
{
    unsigned char SOI;
    int coord_x;
    int coord_y;
    int coord_z;
    unsigned char state;
    unsigned char checksum;
    unsigned char EOI;
} Joystick_InfoTypeDef;


typedef struct _CONTROL_COF	//用于运动部分参数配置
{
//数据定义
//	最大向前速度
//	最大向后速度
//	最大角速度
//	最大加速度
//	最大角加速度
//	保留*6
    float enable_control_debug_flag;
    float enable_angular_speed_control_flag;
    float motor_driver_P;
    float motor_driver_I;
    float motor_driver_D;
    float angular_speed_P;
    float angular_speed_I;
    float angular_speed_D;
    float config[11];
} Control_COFTypeDef;

typedef struct _CHASSIS_SPEED
{
    float	linear_speed;
} Chassis_SpeedTypeDef;

typedef struct _CHASSIS_MILEAGE
{
    float	mileage;
    float total_mileage;
} Chassis_MileageTypeDef;


typedef struct _CHASSIS_SPEEDLIMIT
{
    int   speed_limit_enable_flag;
    float	min_linear_limit_speed;
    float	max_linear_limit_speed;
    float	min_angular_limit_speed;
    float	max_angular_limit_speed;

} Chassis_SpeedLimitTypeDef;

typedef struct _TX2_POWEROFF
{
    unsigned char	cmd;
} Tx2_PoweroffTypeDef;

typedef struct STATISTICS_CONtoTX2
{
    int	save_flag;
    int frame_total;
    int heart_singnal;//CON_ID,0
    int state_change;	//CON_ID,25
    int monitor_info;	//CON_ID,27
    int imu;					//CON_ID,29
    int odom;					//CON_ID,30
    int battery_state;//CON_ID,34
    int ultansound;		//CON_ID,33
    int log_up;				//CON_ID,35
    int event_up;			//CON_ID,36
    int control_up;		//CON_ID,37
    int chassis_speed;//CON_ID,38
    int chassis_odom;	//CON_ID,39
    int poweroff;			//CON_ID,40
    int	ultansound_up;//CON_ID,41
    int statistics_snd_42;  //CON_ID,42
    int statistics_rcv_43;	//CON_ID,43
	  int chassis_alarm;//CON_ID 44
	  int control_info;//CON_ID 44
} STATISTICS_CONTOTX2_MSG;
typedef struct STATISTICS_TX2toCON
{
    int	save_flag;
    int frame_total;
    int heart_signal;       //CON_ID,0
    int control_state;			//CON_ID, 25
    int cmd_vel;						//CON_ID, 26
    int cmd_req;						//CON_ID, 27
    int	control_para;
    int speed_limit;				//CON_ID, 29
    int poweroff_res;				//CON_ID, 30
    int ultrasonic_down;    //CON_ID, 31
    int sendTimesignal;			//CON_ID, 1
    int statistics_snd_32; 	//CON_ID,32  added by xbs 2019.5.9
    int statistics_rcv_33;	//CON_ID, 33  added by xbs 2019.5.9
} STATISTICS_TX2toCON_msg;
//====================================================================================================================//

#endif


