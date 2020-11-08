#ifndef _OUTBOARD_SENSOR_H
#define _OUTBOARD_SENSOR_H



#define FRQ_BMS             0.5
#define FRQ_ULTRASONIC      100
#define FRQ_JOYSTICK        100
#define FRQ_WEIGHING        1
#define FRQ_IMU             100
#define FRQ_ODM             100
#define FRQ_CONTROL_SENSOR  100
#define FRQ_SYS_STATE       100
#define FRQ_READ_MOTOR    	50
#define FRQ_READ_TEMP    		1

void initSenorDevice(void);
void readSenorDevice(void);


#endif

