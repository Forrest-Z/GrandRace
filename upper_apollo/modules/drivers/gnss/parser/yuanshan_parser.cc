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
#include "modules/drivers/gnss/parser/yuanshan_messages.h"
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
namespace {

    constexpr size_t BUFFER_SIZE = 256;
    constexpr float FLOAT_NAN = std::numeric_limits<float>::quiet_NaN();
    constexpr double DEG_TO_RAD = M_PI / 180.0;
    constexpr size_t BUFF_MAX = 120;

    struct _datetime
    {
        uint8_t day, month, year;
        uint8_t hours, minutes, seconds;
        uint16_t millis;
        bool valid; //1 = yes, 0 = no
    };

    uint8_t parse_hex(char c)
    {
        if (c < '0')
            return 0;
        if (c <= '9')
            return c - '0';
        if (c < 'A')
            return 0;
        if (c <= 'F')
            return (c - 'A')+10;
        return 0;
    }
    constexpr double azimuth_deg_to_yaw_rad(double azimuth)
    {
        return (90.0 - azimuth) * DEG_TO_RAD;
    }
}// namespace


/************************************************/
class YuanShanParser : public Parser {
public:
    YuanShanParser();
    explicit YuanShanParser(const config::Config& config);
    virtual MessageType GetMessage(MessagePtr* message_ptr);

    class Tokeniser
    {
    public:
        Tokeniser(char* str, char token);
        bool next(char* out, int len);

    private:
        char* str;
        char token;
    };

    float latitude, longitude;
    float hdop; //positional, horizontal and vertical dilution of precision

    uint16_t speed, course;
    int dop_age;
    int8_t sats_in_use;
    _datetime datetime;

private:
    bool check_checksum();
    char buf[BUFF_MAX];

    void readGN_gga();//GPS定位信息
    void readNG_rmc();  //GPRMC：推荐最小定位信息

    raw_t raw_;  // used for observation data
    Parser::MessageType PrepareMessage(MessagePtr* message_ptr);

private:
    std::vector<char> buffer_;
    std::string line;
 
    size_t total_length_ = 0;
    ::apollo::drivers::gnss::Gnss gnss_;
    ::apollo::drivers::gnss::Ins ins_;
};

/*==============================================================*/
//初始化类Tokeniser
YuanShanParser::Tokeniser::Tokeniser(char* _str, char _token)
{
    str = _str;
    token = _token;
}

bool YuanShanParser::Tokeniser::next(char* out, int len)
{
    uint8_t count = 0;

    if(str[0] == 0)
    return false;

    while(true)
    {
        if(str[count] == '\0')
        {
            out[count] = '\0';
            str = &str[count];
            return true;
        }

    if(str[count] == token)
    {
        out[count] = '\0';
        count++;
        str = &str[count];
        return true;
    }

    if(count < len)
        out[count] = str[count];

    count++;
    }
    return false;
}
/*==============================================================*/

/**********************************************************
 * @模块入口 构造类
 * 
 **********************************************************/
Parser* Parser::create_yuanshan(const config::Config& config)
{
    // void(config);
    return new YuanShanParser();
    //return new YuanShanParser(config);
}

YuanShanParser::YuanShanParser() 
{
  buffer_.reserve(BUFFER_SIZE);
  ins_.mutable_position_covariance()->Resize(9, FLOAT_NAN);
  ins_.mutable_euler_angles_covariance()->Resize(9, FLOAT_NAN);
  ins_.mutable_linear_velocity_covariance()->Resize(9, FLOAT_NAN);

 //apollo is write 1
  if (1 != init_raw(&raw_)) 
  {
    AFATAL << "memory allocation error for observation data structure.";
  }
}

YuanShanParser::YuanShanParser(const config::Config& config) 
{
  buffer_.reserve(BUFFER_SIZE);
  ins_.mutable_position_covariance()->Resize(9, FLOAT_NAN);
  ins_.mutable_euler_angles_covariance()->Resize(9, FLOAT_NAN);
  ins_.mutable_linear_velocity_covariance()->Resize(9, FLOAT_NAN);

 //apollo is write 1
  if (1 != init_raw(&raw_)) 
  {
    AFATAL << "memory allocation error for observation data structure.";
  }
}

/***********************************************************
 * 
***********************************************************/
Parser::MessageType YuanShanParser::GetMessage(MessagePtr* message_ptr) 
{
  if (data_ == nullptr) 
  {
    return MessageType::NONE;
  } 
/* data example:
    $GNGGA,055136.00,3023.81376766,N,10404.32368675,E,7,10,1.4,449.1208,M,0.0000,M,00,0000*41
    $GNRMC,055136.00,A,3023.81376766,N,10404.32368675,E,0.035,71.4,020818,0.0,E,A*1F
*/
  while (data_ < data_end_) 
  {
      buffer_.push_back(*data_);
      line+=*data_;
      data_++;
      total_length_++;

    if(*data_ == '\n') //linefeed
    {
        strcpy(buf, line.c_str()); 
        MessageType type = PrepareMessage(message_ptr);
        buffer_.clear();
        line.clear();
        memset(buf, '\0', BUFF_MAX);
        total_length_ = 0;
        if (type != MessageType::NONE) 
        {
            return type;
        }
    }

    if(buffer_.size() >= BUFF_MAX) //avoid a buffer overrun
    {
        buffer_.clear();
        line.clear();
        memset(buf, '\0', BUFF_MAX);
        total_length_ = 0;
    }
  } 
  return MessageType::NONE;
}

Parser::MessageType YuanShanParser::PrepareMessage(MessagePtr* message_ptr) 
{
    //otherwise, what sort of message is it
    if(strncmp(&buf[1], "$GNRMC", 6) == 0)
    {
        if(!check_checksum()) //if checksum is bad
        {
            AERROR << "check_checksum check failed.";
            return MessageType::NONE; //return
        }
        readNG_rmc();
        *message_ptr = &ins_;
        return MessageType::INS;
    }
    if(strncmp(&buf[1], "$GNGGA", 6) == 0)
    {
        if(!check_checksum()) //if checksum is bad
        {
            AERROR << "check_checksum check failed.";
            return MessageType::NONE; //return
        }
        readGN_gga();//全球定位数据）
        *message_ptr = &ins_;
        return MessageType::INS;
    }
    return MessageType::NONE;
}

/**************************************************************************************
//[ $GNRMC,055136.00,A,3023.81376766,N,10404.32368675,E,0.035,71.4,020818,0.0,E,A*1F ]
*************************************************************************************/
void YuanShanParser::readNG_rmc()
{
    uint16_t counter = 0;
    char token[20];
    Tokeniser tok(buf, ',');

    while(tok.next(token, 20))
    {
        switch(counter)
        {
            case yuanshan::RMC_UTC:
            {
                float time = atof(token);
                int hms = int(time);

                datetime.millis = time - hms;
                datetime.seconds = fmod(hms, 100);
                hms /= 100;
                datetime.minutes = fmod(hms, 100);
                hms /= 100;
                datetime.hours = hms;
                break;
            }
            case yuanshan::RMC_STATUS:
            {
                if(token[0] == 'A')
                {
                    datetime.valid = true;
                    // AINFO<<"GNRMC 定位有效";
                }              
                else if(token[0] == 'V')
                {
                    datetime.valid = false;
                    // AINFO<<"GNRMC 定位无效";
                }
                break;
            }
            case yuanshan::RMC_LAT_DIR:
            {
                //ddmm.mmmm
                float llat = atof(token);

                int ilat = llat/100;
                double latmins = fmod(llat, 100);
                latitude = ilat + (latmins/60);
                break; 
            }
            case yuanshan::RMC_LAT:
            {
                if(token[0] == 'S')
                    latitude = -latitude;
                    
                ins_.mutable_position()->set_lat(latitude);
                // gnss_.mutable_position()->set_lon(latitude);
                break;
            }
            case yuanshan::RMC_LON_DIR:
            {
                float llong = atof(token);
                float ilat = llong/100;
                double lonmins = fmod(llong, 100);
                longitude = ilat + (lonmins/60);
                 break;
            }
            case yuanshan::RMC_LON:
            {
                if(token[0] == 'W')
                    longitude = -longitude;

                ins_.mutable_position()->set_lon(longitude);
                gnss_.mutable_position()->set_lon(longitude);
                break;
            }
            case yuanshan::RMC_GLOBLE_SPEED:
            {
                // 字段 7：速度，节，Knots（一节也是1.852千米／小时）
                speed = atof(token);
                // AINFO<<"地面速度="<<speed;
                break;
            }
            case yuanshan::RMC_HEAND:
            {
                //字段 8：方位角，度（二维方向指向，相当SYS_CMP盘）
                course = atof(token);
                // AINFO<<"地面航向="<<course;
                double yaw = azimuth_deg_to_yaw_rad(course);
                gnss_.mutable_linear_velocity()->set_x(speed * cos(yaw));
                gnss_.mutable_linear_velocity()->set_y(speed * sin(yaw));
                gnss_.mutable_linear_velocity()->set_z(0);//垂直速度
                break;
            }   
            case yuanshan::RMC_DATA_TIME:
            {
                ins_.mutable_header()->set_timestamp_sec(ros::Time::now().toSec());
                break;
            }  
        }
        counter++;
    }
}
/*************************************************************************************/
// GNGGA 
// UTC时间,纬度,纬半球,经度,经半球,定位质量标,卫星数量,水平精确度,天、地水准高度，差分GPS数据等, *, 校验和, 回车、换行
/*************************************************************************************/
void YuanShanParser::readGN_gga()
{
    int counter = 0;
    char token[20];
    Tokeniser tok(buf, ',');

    while(tok.next(token, 20))
    {
        switch(counter)
        {
            case yuanshan::GGA_UTC: //time
            {
                float time = atof(token);
                int hms = int(time);

                datetime.millis = time - hms;
                datetime.seconds = fmod(hms, 100);
                hms /= 100;
                datetime.minutes = fmod(hms, 100);
                hms /= 100;
                datetime.hours = hms;
                
                break;
            }
            case yuanshan::GGA_LAT: //latitude
            {
                float llat = atof(token);
                int ilat = llat/100;
                double mins = fmod(llat, 100);
                latitude = ilat + (mins/60);
                break;
            }
            case yuanshan::GGA_LAT_DIR: //north/south
            {
                if(token[0] == 'S')
                    latitude = -latitude;
                    
                ins_.mutable_position()->set_lat(latitude);
                gnss_.mutable_position()->set_lat(latitude);
                break;
            }
            case yuanshan::GGA_LON: //longitude
            {
                float llong = atof(token);
                int ilat = llong/100;
                double mins = fmod(llong, 100);
                longitude = ilat + (mins/60);
                break;
            }
            case yuanshan::GGA_LON_DIR: //east/west
            {
                if(token[0] == 'W')
                    longitude = -longitude;
                
                ins_.mutable_position()->set_lon(longitude);
                gnss_.mutable_position()->set_lon(longitude);

            }
            break;
            case yuanshan::GGA_STATUS:
            {
                //gps 状态 
                uint32_t fixtype = (atoi(token));
                if(fixtype==0)
                {
                    // AINFO<<"GGA 定位质量标定位无效 NONE INVALID";
                    gnss_.set_type(apollo::drivers::gnss::Gnss::INVALID);
                    ins_.set_type(apollo::drivers::gnss::Ins::INVALID);
                }
                else if(fixtype==1)
                {
                    // AINFO<<"GGA 定位质量标定位有效 GPS CONVERGING";
                    gnss_.set_type(apollo::drivers::gnss::Gnss::SINGLE);
                    ins_.set_type(apollo::drivers::gnss::Ins::CONVERGING);
                }
                else if(fixtype==2)
                {
                    // AINFO<<"GGA 定位质量标定位有效 DIFF PSRDIFF ";
                    ins_.set_type(apollo::drivers::gnss::Ins::GOOD);
                    gnss_.set_type(apollo::drivers::gnss::Gnss::PSRDIFF);
                }
                else if(fixtype==4)//固定解
                {
                    gnss_.set_type(apollo::drivers::gnss::Gnss::RTK_INTEGER);
                    ins_.set_type(apollo::drivers::gnss::Ins::GOOD);
                }
                else if(fixtype==5)
                {
                    gnss_.set_type(apollo::drivers::gnss::Gnss::RTK_FLOAT);
                    ins_.set_type(apollo::drivers::gnss::Ins::CONVERGING);
                }
                else
                {
                    ins_.set_type(apollo::drivers::gnss::Ins::INVALID);
                }

                break;
            }
            case yuanshan::GGA_STAS_NUM:
            {
                sats_in_use = atoi(token);

                // AINFO<<"使用卫星数量，从00到12="<<sats_in_use;
                gnss_.set_num_sats(sats_in_use);
                break;
            }
            case yuanshan::GGA_HDOP:
            {
                hdop = atoi(token);
                // AINFO<<"水平精确度，0.5到99.9。="<<hdop;
                
                break;
            }
            case yuanshan::GGA_HIGHT:
            {
                float new_alt = atof(token);
                gnss_.mutable_position()->set_height(new_alt);

                //AINFO<<"天、地水准高度="<<new_alt;
                ins_.mutable_position()->set_height(new_alt);
                ins_.mutable_header()->set_timestamp_sec(ros::Time::now().toSec());

                break;
            }
        }
        counter++;
    }
}

/************************
校验函数
************************/
bool YuanShanParser::check_checksum()
{
    uint8_t sum_check = 0,sum = 0;
    //[ $GNRMC,055136.00,A,3023.81376766,N,10404.32368675,E,0.035,71.4,020818,0.0,E,A*1F ]
    //代表了“$”和“*”之间所有字符的按位异或值
    if (buf[strlen(buf)-4] == '*')
    {
        //获取检验值
        sum = parse_hex(buf[strlen(buf)-3]) * 16;
        sum += parse_hex(buf[strlen(buf)-2]);
        for (size_t i=0; i < (strlen(buf)-6); i++)
        {
            sum_check ^= buf[i+2];
        }
  
        if (sum != sum_check)
        {
            return false;
        }
        return true;
    }
    return false;
}

}  // namespace gnss
}  // namespace drivers
}  // namespace apollo