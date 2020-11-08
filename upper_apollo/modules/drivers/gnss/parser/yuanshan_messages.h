#ifndef MODULES_DRIVERS_GNSS_YUANSHAN_MESSAGES_H_
#define MODULES_DRIVERS_GNSS_YUANSHAN_MESSAGES_H_

#include <stdint.h>
#include "modules/drivers/gnss/proto/config.pb.h"

namespace apollo {
namespace drivers {
namespace gnss {
namespace yuanshan {

// enum  MessageId : uint8_t 
// {
//   CMD_GPGSV = 1,
//   CMD_BDGSV = 2,
//   CMD_GNGGA = 3,
//   CMD_GNGSA = 4,
//   CMD_GNRMC = 5,
// };

// enum class SolutionType : uint32_t 
// {
//   NONE      = 0,//无效解
//   SINGLE    = 1,//单点解
//   PSRDIFF   = 2,//伪距差分解
//   RT2       = 4,//固定解
//   RTK_FLOAT = 5,//浮点解
// };


// enum _fix 
// { 
//     FIX_NONE = 1, 
//     FIX_2D, 
//     FIX_3D 
// };
// enum _op_mode 
// { 
//     MODE_MANUAL, 
//     MODE_AUTOMATIC 
// };

// struct satellite
// {
//     uint8_t prn;
//     int16_t elevation;
//     int16_t azimuth;
//     uint8_t snr; //signal to noise ratio
// };

enum GnRmcCmd:uint16_t
{ 
  RMC_RMC = 0,
  RMC_UTC = 1, 
  RMC_STATUS =2,
  RMC_LAT_DIR  =3,
  RMC_LAT =4,
  RMC_LON_DIR  =5,
  RMC_LON =6,
  RMC_GLOBLE_SPEED =7,
  RMC_HEAND =8,
  RMC_DATA_TIME =9,
};
enum GnGgaCmd:uint16_t
{ 
  GGA_GGA = 0,
  GGA_UTC = 1, 
  GGA_LAT = 2,
  GGA_LAT_DIR  = 3,
  GGA_LON = 4,
  GGA_LON_DIR  = 5,
  GGA_STATUS =6,
  GGA_STAS_NUM =7,
  GGA_HDOP =8,
  GGA_HIGHT = 9,
};



/*===================================================*/

}  // namespace yuanshan
}  // namespace gnss
}  // namespace drivers
}  // namespace apollo

#endif  // MODULES_DRIVERS_GNSS_YUANSHAN_MESSAGES_H_
