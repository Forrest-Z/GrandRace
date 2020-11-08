#ifndef __INFO_OD_H__
#define __INFO_OD_H__

#include <stdio.h>
#include <stdlib.h>
#include "info_callback.h"
#include "callback.h"
#include "buffer.h"
#include "info_core.h"

extern BUFFER_DATA *lin_buffer_data[];
extern const HEART_SINGAL_STRUCT heart_singal_struct[];
extern const Publish_struct heartSingal[];

extern PUBLISH_FRAME_STRUCT TX2_heartSignal_frame;
extern PUBLISH_FRAME_STRUCT power_board_heartSignal_frame;
extern PUBLISH_FRAME_STRUCT sensor_board_heartSignal_frame;
extern PUBLISH_FRAME_STRUCT ultrasonic_board_heartSignal_frame;
extern PUBLISH_FRAME_STRUCT lightstrip_board_heartSignal_frame;

extern HEART_SINGAL_TIME_STRUCT TX2_heart_singal_time;
extern HEART_SINGAL_TIME_STRUCT power_board_heart_singal_time;
extern HEART_SINGAL_TIME_STRUCT joy_board_heart_singal_time;
extern HEART_SINGAL_TIME_STRUCT ultrasonic_board_heart_singal_time;
extern HEART_SINGAL_TIME_STRUCT lightstrip_board_heart_singal_time;

int cal_lin_buffer_size(void);
int cal_heartSingal_size(void);
#endif
