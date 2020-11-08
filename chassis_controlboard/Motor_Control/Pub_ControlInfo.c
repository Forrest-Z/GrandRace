#include "Pub_ControlInfo.h"

#include "joystick.h"

void pub_controlInfo(void)
{
	unsigned long int t = 0;
	ControlData_Typedef controlDataCopy;
	updateControldata(&controlDataCopy);
	
	//摇杆的
	Joystick_TypeDef joyDataCopy;
	getJoystickData(&joyDataCopy);
	
	
	getSystemTimeStamp(t++, &Control_Info_Pub_Buffer.time);	
	Control_Info_Pub_Buffer.frequncy = 1.0f / controlDataCopy.Ts;
	Control_Info_Pub_Buffer.linear_vel = joyDataCopy.real_y;
	Control_Info_Pub_Buffer.angular_del = joyDataCopy.real_x;
	publishFrameData(TX2_ID,CONTROL_INFO_ID, (unsigned char *)&Control_Info_Pub_Buffer, sizeof(Control_InfoTypeDef));
}

