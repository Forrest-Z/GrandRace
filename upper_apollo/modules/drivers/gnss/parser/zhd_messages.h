#ifndef MODULES_DRIVERS_GNSS_ZHD_MESSAGES_H_
#define MODULES_DRIVERS_GNSS_ZHD_MESSAGES_H_

#include <stdint.h>
#include "modules/drivers/gnss/proto/config.pb.h"

namespace apollo {
namespace drivers {
namespace gnss {
namespace zhd{

#pragma pack(1)//注意，一般n只取1，2，4等，取其它值编译器应该会自动把n变成接近上述值的值。
/*===================================================*/
enum SyncByte : uint8_t 
{
  SYNC_0 = 0xAA,
  SYNC_1 = 0x33,
  VERSION_2 = 0x02,
  VERSION_3 = 0x00,
};

// Fix_type 值,代表 RTK 定位解状态,Fix_type=4 为定位固定解有效工作状态
enum class _fixtype : uint32_t 
{
  FIX_TYPE_NONE = 0, //Fix not available or invalid
  FIX_TYPE_SPOINT =1, //Single point
  FIX_TYPE_VBS_PPP =2,// Pseudorange differential ,Unconverged OmniSTAR,HP/XP/G2/VBS converging PPP
  FIX_TYPE_RT2 =4,  //RTK fixed ambiguity solution (RT2)(固定解有效工作状态)
  FIX_TYPE_PPP =5,  //Converged PPP
  FIX_TYPE_DEAD_MOD =6,  //Dead reckoning mode
  FIX_TYPE_INPUT_MOD =7,  //Manual input mode (fixed position)
};
// Fix_type 值,代表 RTK 定位解状态,Fix_type=4 为定位固定解有效工作状态
enum class _PosType : uint8_t 
{
  POS_TYPE_NONE = 0, 
  POS_TYPE_FIXEDPOS =1, 
  FIX_TYPE_FIXEDHEIGHT =2,
  POS_TYPE_DOPPLER_VELOCITY =8, 
  POS_TYPE_SINGLE =16,  
  POS_TYPE_PSRDIFF =17,  
  POS_TYPE_WAAS =18, 
  FIX_TYPE_PROPAGATED =19,
  POS_TYPE_OMNISTAR =20, 
  POS_TYPE_L1_FLOAT =32,  
  POS_TYPE_IONOFREE_FLOAT =33,  
  POS_TYPE_NARROW_FLOAT =34, 
  FIX_TYPE_L1_INT =48,
  POS_TYPE_NARROW_INT =50, 
  POS_TYPE_OMNISTAR_HP=64,  
  POS_TYPE_OMNISTAR_XP =65,  
  POS_TYPE_PPP_CONVERGING =68, 
  POS_TYPE_PPP =69, 
  POS_TYPE_INS_Angle_state =99, 
};


typedef struct 
{
  unsigned char head[2];          //2 bytes deviation 0 数据头标志
  unsigned short int version;     //2 bytes deviation 2 格式版本 0x02 00 版本从 1 开始(当前 版本为 2)
  unsigned short int length;      //2 bytes deviation 4 数据包总长度( 0~65535)
  unsigned short int freq;        //2 bytes deviation 6 数据输出频率
  float time_utc;                 //4 bytes deviation 8 UTC 时间
  unsigned short int year_utc;    //2 bytes deviation 12 UTC 年份
  unsigned short int month_utc;   //2 bytes deviation 14 UTC 月份 
  unsigned short int day_utc;     //2 bytes deviation 16 UTC 日
  unsigned short int hour_utc;    //2 bytes deviation 18 UTC 时
  unsigned short int min_utc;     //2 bytes deviation 20 UTC 分
  unsigned short int sec_utc;     //2 bytes deviation 22 UTC 秒 十毫秒) 1220  代 表 12200ms
  double latitude;                //8 bytes deviation 24 纬度(度)
  double longitude;               //8 bytes deviation 32 经度(度)
  double altitude;                //8 bytes deviation 40 海拔高(m)
  float eph;                      //4 bytes deviation 48 水平误差 (m)
  float epv;                      //4 bytes deviation 52 垂直误差 (m)
  float vel_ground_m_s;           //4 bytes deviation 56 GPS 地 速 m/s
  float angle_tracktrue;          //4 bytes deviation 60  地速方向
  float angle_heading;            //4 bytes deviation 64 偏航角度值(0°~359.999°)
  float angle_pitch;              //4 bytes deviation 68 俯仰角度值(±90°)
  double vel_n_m_s;                   //8 bytes deviation 72 GPS 北 向速度 m/s
  double vel_e_m_s;                   //8 bytes deviation 80 GPS 东 向速度 m/s
  double vel_u_m_s;                   //8 bytes deviation 88 GPS 天 向速度,向上为正 m/s
  unsigned short int satellites_used;   //2 bytes deviation 96 使用卫星数
  unsigned short int satellites_track;  //2 bytes deviation 98 可见卫星数
  float vel_ned_valid;                  //4 bytes deviation 100 分别代表北东天速度是否有效
  unsigned short int fix_type;          //2 bytes deviation 104  GPS 状 态
  float angle_postype;                  //4 bytes deviation 106 偏航角状态 偏航角位置类型
  float head_deviation;                 //4 bytes deviation 110 偏航角标准差
  unsigned short int ins_state;         //2 bytes deviation 114 是否启用
  double gnss_alt_delta ;               //8 bytes deviation 116 大地水准面和所选椭球面坐标系之间的高度差值(m)
  double ellipsoidal_h;                 //8 bytes deviation 124 椭球高,测量点与椭球面的正交距离(m)
  unsigned short int diff_age;          //2 bytes deviation 132
  unsigned char reserve[2];             //2 bytes deviation 134
  unsigned short int checksum;          //2 bytes deviation 136
}gps_rtk_zhd_packet_t; //total 138 bytes

#pragma pack()

}  // namespace zhd
}  // namespace gnss
}  // namespace drivers
}  // namespace apollo

#endif  // MODULES_DRIVERS_GNSS_YUANSHAN_MESSAGES_H_