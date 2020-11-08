#include <cmath>
#include <iostream>
#include <limits>
#include <memory>

#include <vector>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "ros/include/ros/ros.h"

#include "modules/common/log.h"
#include "modules/drivers/gnss/parser/zhd_messages.h"
#include "modules/drivers/gnss/parser/parser.h"
#include "modules/drivers/gnss/parser/rtcm_decode.h"
#include "modules/drivers/gnss/proto/gnss.pb.h"
#include "modules/drivers/gnss/proto/gnss_best_pose.pb.h"
#include "modules/drivers/gnss/proto/gnss_raw_observation.pb.h"
#include "modules/drivers/gnss/proto/heading.pb.h"
#include "modules/drivers/gnss/proto/imu.pb.h"
#include "modules/drivers/gnss/proto/ins.pb.h"
#include "modules/drivers/gnss/util/time_conversion.h"
#include "modules/drivers/gnss/parser/rtcm_decode.h"

#include <iomanip>

using namespace std;

namespace apollo {
namespace drivers {
namespace gnss {
// Anonymous namespace that contains helper constants and functions.
namespace {

    #define BUILD_UINT16_T(loByte, hiByte) ((uint16_t)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))	
    #define BUILD_INT16_T(loByte, hiByte)  ((int16_t) (((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))

    constexpr size_t BUFFER_SIZE = 256;
    constexpr double DEG_TO_RAD = M_PI / 180.0;
    
    constexpr float FLOAT_NAN = std::numeric_limits<float>::quiet_NaN();

    struct _datetime
    {
        uint8_t day, month, year;
        uint8_t hours, minutes, seconds;
        uint16_t millis;
        bool valid; //1 = yes, 0 = no
    };

    constexpr double azimuth_deg_to_yaw_rad(double azimuth)
    {
      return (- azimuth) * DEG_TO_RAD;  //90.0 
    }

}// namespace

class ZhdParser : public Parser {
public:
    ZhdParser();
    explicit ZhdParser(const config::Config& config);

    virtual MessageType GetMessage(MessagePtr* message_ptr);

private:

    bool check_checksum();

    raw_t raw_;  // used for observation data
    Parser::MessageType PrepareMessage(MessagePtr* message_ptr);
      // -1 is an unused value.
    zhd::_fixtype fixtype_status = static_cast<zhd::_fixtype>(-1);
    zhd::_PosType position_type_ = static_cast<zhd::_PosType>(-1);

    Parser::MessageType prepare_message(MessagePtr& message_ptr);

    std::vector<uint8_t> buffer_;
    size_t total_length_ = 0;
    uint8_t buffer[BUFFER_SIZE];

    // ::apollo::drivers::gnss::Gnss gnss_;
    // ::apollo::drivers::gnss::Imu  imu_;
    ::apollo::drivers::gnss::Ins  ins_;
};


/**********************************************************
 * @模块入口 构造类
 * 
 **********************************************************/
Parser* Parser::createZhd(const config::Config& config)
{
    return new ZhdParser();
    //return new ZhdParser(config);
}
ZhdParser::ZhdParser() 
{
  buffer_.reserve(BUFFER_SIZE);
  ins_.mutable_position_covariance()->Resize(9, FLOAT_NAN);
  ins_.mutable_euler_angles_covariance()->Resize(9, FLOAT_NAN);
  ins_.mutable_linear_velocity_covariance()->Resize(9, FLOAT_NAN);

  if (1 != init_raw(&raw_)) 
  {
    AFATAL << "memory allocation error for observation data structure.";
  }
}

ZhdParser::ZhdParser(const config::Config& config) 
{
  buffer_.reserve(BUFFER_SIZE);
  ins_.mutable_position_covariance()->Resize(9, FLOAT_NAN);
  ins_.mutable_euler_angles_covariance()->Resize(9, FLOAT_NAN);
  ins_.mutable_linear_velocity_covariance()->Resize(9, FLOAT_NAN);

  if (1 != init_raw(&raw_)) 
  {
    AFATAL << "memory allocation error for observation data structure.";
  }
}
/***********************************************************
 * 数据头      格式版本   数据长度  数据域...    校验值
 *0xAA 33     0x00 02   0xXX XX 0x0A ...    0xXX
***********************************************************/
Parser::MessageType ZhdParser::GetMessage(MessagePtr* message_ptr) 
{
  if (data_ == nullptr) 
  {
    return MessageType::NONE;
  } 

  while (data_ < data_end_) 
  {
    if (buffer_.size() == 0) 
    {  // Looking for SYNC0
      if (*data_ == zhd::SYNC_0) 
      {
        buffer[0]=zhd::SYNC_0;
        buffer_.push_back(*data_);
      }
      ++data_;
    }
    else if (buffer_.size() == 1) 
    {  // Looking for SYNC1
      if (*data_ == zhd::SYNC_1) 
      {
        buffer[1]=zhd::SYNC_1;
        buffer_.push_back(*data_++);
      } 
      else 
      {
        buffer_.clear();
      }

    }
    else if (buffer_.size() == 2) 
    {  // Looking for VERSION_2
      if (*data_ == zhd::VERSION_2) 
      {
        buffer[2]=*data_;
        buffer_.push_back(*data_++);
      } 
      else 
      {
        buffer_.clear();
      }
    }
    else if (buffer_.size() == 3) 
    {  // Looking for VERSION_3
      if (*data_ == zhd::VERSION_3) 
      {
        buffer[3]=*data_;
        buffer_.push_back(*data_++);
        total_length_ = 6;
      } 
      else 
      {
        buffer_.clear();
        total_length_ = 0;
      }
    }
    else if (total_length_ > 0) 
    {
        if(buffer_.size() >256)
        {
          buffer_.clear();
          total_length_ = 0;
        }
        if (buffer_.size() < total_length_) 
        {  // Working on body.
            buffer[buffer_.size()]=*data_;
            buffer_.push_back(*data_++);
            if(buffer_.size() ==6)
            {
              //当前数据包长度,除去校验位的所有数据 
              // total_length_ = 数据长度＋２
              total_length_= *reinterpret_cast<uint16_t*>(buffer_.data() + 4) +2;
              if(total_length_ > 138) total_length_ = 138;
              //AINFO<<"total_length_="<<hex<<total_length_;
            }
            continue;
        }
        MessageType type = PrepareMessage(message_ptr);
        buffer_.clear();
        total_length_ = 0;
        if (type != MessageType::NONE) 
        {
            return type;
        }
    }
  }
  return MessageType::NONE;
}


Parser::MessageType ZhdParser::PrepareMessage(MessagePtr* message_ptr) 
{
    if(!check_checksum()) //if checksum is bad
    {
        AERROR << "check_checksum check failed.";
        return MessageType::NONE; //return
    }

    auto stZhdData =reinterpret_cast<const zhd::gps_rtk_zhd_packet_t*>(buffer_.data());
    ins_.mutable_position()->set_lon(stZhdData->longitude);
    ins_.mutable_position()->set_lat(stZhdData->latitude);
    ins_.mutable_position()->set_height(stZhdData->altitude);

  // ins_.mutable_euler_angles()->set_x(pva->roll * DEG_TO_RAD);
  // ins_.mutable_euler_angles()->set_y(-pva->pitch * DEG_TO_RAD);
  // ins_.mutable_euler_angles()->set_z(azimuth_deg_to_yaw_rad(pva->azimuth));

    ins_.mutable_linear_velocity()->set_x(stZhdData->vel_e_m_s);
    ins_.mutable_linear_velocity()->set_y(stZhdData->vel_n_m_s);
    ins_.mutable_linear_velocity()->set_z(stZhdData->vel_u_m_s);

    // AINFO << "stZhdData->angle_heading)"<<stZhdData->angle_heading;
    ins_.mutable_euler_angles()->set_z(azimuth_deg_to_yaw_rad(stZhdData->angle_heading));
    ins_.mutable_euler_angles()->set_x(0);
    ins_.mutable_euler_angles()->set_y(0);
 

    ins_.mutable_header()->set_timestamp_sec(ros::Time::now().toSec());

    // AINFO << "stZhdData->altitude)"<<stZhdData->altitude;

    *message_ptr = &ins_;
    return MessageType::INS;
    
    // return MessageType::NONE;
}

bool ZhdParser::check_checksum()
{
  // uint16_t xor_cheack = 0; 
  unsigned short int xor_cheack=0;
  uint16_t data_len = 0;
  uint16_t Checksum = 0;

  data_len =total_length_-2;
  Checksum = (uint16_t)*reinterpret_cast<uint16_t*>(buffer_.data() + data_len);
  for(size_t i = 0; i < (data_len); i++) 
  {
    /*当前数据包校验位 进行异或校验*/
    xor_cheack = xor_cheack^(*reinterpret_cast<uint8_t*>(buffer_.data() + i));
    // AINFO<<"["<<i<<"]"<<hex<<(uint8_t)*reinterpret_cast<uint8_t*>(buffer_.data() + i)<<"buffer[i]="<<hex<<buffer[i];
  }

  if (Checksum != xor_cheack)
  {
    //test
    auto stZhd = reinterpret_cast<const zhd::gps_rtk_zhd_packet_t*>(buffer_.data());
    AERROR<<"Checksum ="<< Checksum <<" xor_cheack="<<xor_cheack<<"stZhd_checksum="<<stZhd->checksum;
  //  AERROR << " total_length_ = " <<total_length_;
//    AERROR << " data_len = " <<data_len;

    //for(size_t i = 0; i < (data_len); i++) 
   // {
      /*当前数据包校验位 进行异或校验*/
     // AERROR<<"["<<i<<"]"<<hex<<(uint8_t)*reinterpret_cast<uint8_t*>(buffer_.data() + i);
   // }
    return false;
  }
  // AINFO<<" ====================Checksum= OK";
  return true;
}

}  // namespace gnss
}  // namespace drivers
}  // namespace apollo
