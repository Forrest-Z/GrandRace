#include "outboard_sensor.h"
#include "Clock.h"
#include "imu.h"
#include "odom.h"
#include "tx2_power.h"
#include "sys_monitor.h"
#include "sys_cpu.h"
#include "watchdog_task.h"
#include "joystick.h"
#include "imu_driver.h"
#include "motor_driver.h"
#include "idea.h"
#include "temp_sensor.h"
#include "Zhd.h"
void initSenorDevice(void)
{
    initImuDevice();
    initSystemMonitor();
		init_temp_sensor();	//为了读取温度初始化
}

void readSenorDevice(void)
{
    static uint32_t Outboard_time[8] = {0x00};

    //读取CPU使用率参数
    readSystemCpuPara();

    if (getSystemTick() > Outboard_time[0])
    {
        readImuDevice();
				calGravityToAxis();//属于姿态解算的一部分，功能有公性，故放在这里
        Outboard_time[0] += 1000 / FRQ_IMU;
    }

    if (getSystemTick() > Outboard_time[1])
    {
        readOdomDevice();        
				Outboard_time[1] += 1000 / FRQ_ODM;
    }

    if (getSystemTick() - Outboard_time[2] >= (1000 / FRQ_SYS_STATE))
    {
        readSystemMonitor();
        Outboard_time[2] = getSystemTick();

    }
    if (getSystemTick() - Outboard_time[3] >= (1000 / FRQ_JOYSTICK))
    {
        readJoystickDevice();
        Outboard_time[3] = getSystemTick();
    }
    if (getSystemTick() - Outboard_time[4] >= (1000 / 1))
    {
        HAL_GPIO_TogglePin(GPIOG, LED_GREEN_Pin);
        Outboard_time[4] = getSystemTick();
    }
    if (getSystemTick() - Outboard_time[5] >= (1000 / FRQ_READ_MOTOR))
    {
        readMotorDevice();
        Outboard_time[5] = getSystemTick();
    }
		
		/*2019/8/7 使用了一种不同的方法获取做周期处理
			优点：
				1.更加精确的周期控制
						以前的方法会导致实际频率比预期的更低
				2.更快的执行效率
						少调用了一次时间函数
						少一次减法，但多一次加法				
		*/
		if (getSystemTick() > Outboard_time[6])
    {
        read_temp_sensor();
        Outboard_time[6] += (1000 / FRQ_READ_TEMP);
    }
		
		if (getSystemTick() > Outboard_time[7])
    {
				zhd_sndshftpos();
				zhd_sndVehSpdF();
				zhd_sndVehSpdR();
        Outboard_time[7] += (1000 / 20);
    }
}


