/******************************************************************************
 * Copyright 2018 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/
#include "modules/guardian/guardian.h"

#include <cmath>

#include "modules/common/adapters/adapter_gflags.h"
#include "modules/common/adapters/adapter_manager.h"
#include "modules/common/log.h"
#include "modules/guardian/common/guardian_gflags.h"
#include "ros/include/ros/ros.h"

namespace apollo {
namespace guardian {

using apollo::canbus::Chassis;
using apollo::common::ErrorCode;
using apollo::common::Status;
using apollo::common::adapter::AdapterManager;
using apollo::control::ControlCommand;
using apollo::guardian::GuardianCommand;
using apollo::monitor::SystemStatus;

std::string Guardian::Name() const { return FLAGS_module_name; }

Status Guardian::Init() {
  AdapterManager::Init(FLAGS_adapter_config_filename);
  CHECK(AdapterManager::GetChassis()) << "Chassis is not initialized.";
  CHECK(AdapterManager::GetSystemStatus())
      << "SystemStatus is not initialized.";
  CHECK(AdapterManager::GetControlCommand()) << "Control is not initialized.";
  AINFO << "Control is not initialized.";
  return Status::OK();
}

Status Guardian::Start() {
  AdapterManager::AddChassisCallback(&Guardian::OnChassis, this);
  AdapterManager::AddSystemStatusCallback(&Guardian::OnSystemStatus, this);
  AdapterManager::AddControlCommandCallback(&Guardian::OnControl, this);
  const double duration = 1.0 / FLAGS_guardian_cmd_freq;
  timer_ = AdapterManager::CreateTimer(ros::Duration(duration),
                                       &Guardian::OnTimer, this);
  return Status::OK();
}

void Guardian::Stop() { timer_.stop(); }

void Guardian::OnTimer(const ros::TimerEvent&) {
  //ADEBUG << "Timer is triggered: publish Guardian result";
  AINFO << "Timer is triggered: publish Guardian result";
  bool safety_mode_triggered = false;
  if (FLAGS_guardian_enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    safety_mode_triggered = system_status_.has_safety_mode_trigger_time();
  }

  safety_mode_triggered = true;
  if (safety_mode_triggered) {
    AINFO << "Safety mode triggered, enable safty mode";
    //TriggerSafetyMode();
    TriggerSafety();
  } else {
    AINFO << "Safety mode not triggered, bypass control command";
    PassThroughControlCommand();
  }

  AdapterManager::FillGuardianHeader(FLAGS_node_name, &guardian_cmd_);
  AdapterManager::PublishGuardian(guardian_cmd_);
  
}

void Guardian::OnChassis(const Chassis& message) {
  AINFO << "Received chassis data: run chassis callback.";
  std::lock_guard<std::mutex> lock(mutex_);
  chassis_.CopyFrom(message);
}

void Guardian::OnSystemStatus(const SystemStatus& message) {
  AINFO << "Received monitor data: run monitor callback.";
  std::lock_guard<std::mutex> lock(mutex_);
  system_status_.CopyFrom(message);
}

void Guardian::OnControl(const ControlCommand& message) {
  AINFO << "Received control data: run control command callback.";
  std::lock_guard<std::mutex> lock(mutex_);
  control_cmd_.CopyFrom(message);
}

void Guardian::PassThroughControlCommand() {
  std::lock_guard<std::mutex> lock(mutex_);
  guardian_cmd_.mutable_control_command()->CopyFrom(control_cmd_);
}

void Guardian::TriggerSafetyMode() {
  AINFO << "Safety state triggered, with system safety mode trigger time : "
        << system_status_.safety_mode_trigger_time();
  std::lock_guard<std::mutex> lock(mutex_);
  bool sensor_malfunction = false, obstacle_detected = false;
  if (!chassis_.surround().sonar_enabled() ||
      chassis_.surround().sonar_fault()) {
    AINFO << "Ultrasonic sensor not enabled for faulted, will do emergency "
             "stop!";
    sensor_malfunction = true;
  } else {
    // TODO(QiL) : Load for config

    AINFO << "chassis_.surround().sonar_range_size()" << chassis_.surround().sonar_range_size();
    for (int i = 0; i < chassis_.surround().sonar_range_size(); ++i) {
      AINFO << "chassis_.surround().sonar_range(i)" << chassis_.surround().sonar_range(i);
      if ((chassis_.surround().sonar_range(i) > 0.0 &&
           chassis_.surround().sonar_range(i) < 2.5) ||
          chassis_.surround().sonar_range(i) > 30) {
        AINFO << "Object detected or ultrasonic sensor fault output, will do "
                 "emergency stop!";
        obstacle_detected = true;
      }
    }
  }

  guardian_cmd_.mutable_control_command()->set_throttle(0.0);
  guardian_cmd_.mutable_control_command()->set_steering_target(0.0);
  guardian_cmd_.mutable_control_command()->set_steering_rate(0.0);
  guardian_cmd_.mutable_control_command()->set_is_in_safe_mode(true);

  // TODO(QiL) : Remove this one once hardware re-alignment is done.
  sensor_malfunction = false;
  obstacle_detected = false;
  AINFO << "Temporarily ignore the ultrasonic sensor output during hardware "
           "re-alignment!";

  if (system_status_.require_emergency_stop() || sensor_malfunction ||
      obstacle_detected) {
    AINFO << "Emergency stop triggered! with system status from monitor as : "
          << system_status_.require_emergency_stop();
    guardian_cmd_.mutable_control_command()->set_brake(
        FLAGS_guardian_cmd_emergency_stop_percentage);
  } else {
    AINFO << "Soft stop triggered! with system status from monitor as : "
          << system_status_.require_emergency_stop();
    guardian_cmd_.mutable_control_command()->set_brake(
        FLAGS_guardian_cmd_soft_stop_percentage);
  }
}


//wh:ult_safty
void Guardian::TriggerSafety() {

  //增设变量
  double danger_range = 50;  // 设定（停车）危险距离为100 cm
  double brake_range = 70;  // 设定（减速）刹车距离为200 cm
  double warn_range = 80;  // 设定（限速）警告距离为300 cm    
  int flag = 0;  // 障碍物标志位 1 ---0- 安全； 1-限速； 2-刹车； 3-非常危险
  //int flag_ultsafty[]={0};  
  //uint16_t warn_flag_xxxx = 0;  // 障碍物位置标志2
  //uint16_t obs_flag_xxxx = 0;
  double key_linear_x = control_cmd_.linear_velocity();  // get the speed from control(control_cmd)
  double key_angle_z = control_cmd_.angular_velocity();  //
  double linear_velocity = 0.5; //壁障后给底盘的速度
  double angular_velocity = 0.5; 
  double warn_linear_x = 1.0; 
  double warn_angle_z = 1.0; 

  AINFO << "Safety state triggered, with system safety mode trigger time : "
        << system_status_.safety_mode_trigger_time();
  std::lock_guard<std::mutex> lock(mutex_);
  //bool sensor_malfunction = false, obstacle_detected = false;
  
  if (!chassis_.surround().sonar_enabled() ||
     chassis_.surround().sonar_fault() ) { 
    AINFO <<  "Ultrasonic sensor not enabled for faulted, will do emergency "
             "stop!";
  } else {
    for (int i = 0; i < chassis_.surround().sonar_range_size(); i++) {
       AINFO << "get sonar_range data: sonar"<< i << "-->" << chassis_.surround().sonar_range(i);
      if (chassis_.surround().sonar_range(i) > 0.0 &&
           chassis_.surround().sonar_range(i) < danger_range) {
        AINFO << "DANGER!!!";
        //obstacle_detected = true;
        //flag_ultsafty[i] = eArea_danger;
        flag = eArea_danger;
      }else if(chassis_.surround().sonar_range(i) < brake_range ){
        AINFO << "Brake!!";
        //obstacle_detected = true;
        //flag_ultsafty[i] = eArea_brake;
        flag = flag > eArea_brake ? flag : eArea_brake;
      }else if(chassis_.surround().sonar_range(i) < warn_range ){
        AINFO << "Warn!";
        //obstacle_detected = true;
        //flag_ultsafty[i] = eArea_warn;
        flag = flag > eArea_warn ? flag : eArea_warn;
      }else{
        AINFO << "Safe!"; 
        //obstacle_detected = true;
        //flag_ultsafty[i] = eArea_safty;
        flag = flag > eArea_safty ? flag : eArea_safty;
      }
      //检测障碍物具体位置信息，给障碍物位置标志2
      //check_obs_of_Num(i ,&flag_ultsafty ,& warn_flag_xxxx); 
    }
    
    //obs_flag_xxxx = warn_flag_xxxx;
    AINFO << "take action";
    //ROS_INFO("cur warn_flag_xxxx=[%02x]",obs->flag_xxxx);
    switch (flag){
      case eArea_safty: // 0-安全 
        {
          linear_velocity = key_linear_x;
          angular_velocity = key_angle_z;
          AINFO << "安全动作";
          break;
        }
      case eArea_warn:// 1-警告 : 速度不超过警告速度（ < warn_linear_x , < warn_angle_z)
        {
          //linear speed check
          if(key_linear_x >0)
          {
            linear_velocity = key_linear_x > warn_linear_x ? warn_linear_x:key_linear_x;
          }
          else
          {
            linear_velocity = key_linear_x > -warn_linear_x ? key_linear_x:warn_linear_x;
          }
          //angular speed check
          if(key_angle_z >0)
          {
            angular_velocity = key_angle_z > warn_angle_z ? warn_angle_z:key_angle_z;
          }
          else
          {
            angular_velocity = key_angle_z > -warn_angle_z ? key_angle_z:warn_angle_z;
          }
          AINFO << "警告动作";
          break;
        }
      case eArea_brake:// 2-刹车
        {
          // check_breke_ult(&pub_linear_x,&pub_angle_z,obs); 
          linear_velocity = 0.5*key_linear_x;
          angular_velocity  = 0.5*key_angle_z;
          AINFO<<"刹车动作";
          break;
        }
      case eArea_danger:// 3-危险
        {
          linear_velocity = 0;
          angular_velocity  = 0;
          // check_danger_ult(&pub_linear_x,&pub_angle_z,obs);
          AINFO<<"非常危险动作";
          break;
        }
      default: //go forward straight line
        {   
          linear_velocity = key_linear_x;
          angular_velocity = key_angle_z;
          AINFO<<"---> default";
          break;
        }
    }
  //} 
  AINFO << "linear_velocity:" << linear_velocity;
  AINFO << "angular_velocity:" << angular_velocity;
  guardian_cmd_.mutable_control_command()->set_linear_velocity(linear_velocity);
  guardian_cmd_.mutable_control_command()->set_angular_velocity(angular_velocity);
  
}

/********** 函数check_obs_of_Num***********/
/****用于标志障碍物的具体位置 ****************/
/*void Guardian::check_obs_of_Num(int i,int *flag_ultsafty[],uint16_t *warn_flag)
{
  static  uint8_t f_tmp = 0,b_tmp = 0,l_tmp = 0,r_tmp = 0;
  uint8_t f_bSet = 0,b_bSet = 0,l_bSet = 0,r_bSet = 0;
  int warn[4][4]={0};
  for(i=0;i<4;i++)
  {
    warn[eFront][i] = flag_ultsafly[i];
    f_tmp |= flag_ultsafty[i];
    f_bSet = f_tmp > 0 ? 1 :0;
    if ((f_bSet == 1) && (i == 3))
    {
      setbit(*warn_flag, eFront);//不完全
      //print_dbg("warn_flag_xxxv eFront");
      f_tmp = 0;
    }
    else if((f_bSet == 0 ) && (i == 3))
    {
      clrbit(*warn_flag, eFront);//安全清零
      //print_dbg("warn_flag_xxxx eFront");
      f_tmp = 0;
    }
  }
  for(i=4;i<8;i++)
  {
    warn[eBack][i-4] = flag_ultsafty[i];
    b_tmp |= safly_flag[i];
    b_bSet = b_tmp>0 ? 1 :0;
    if ((b_bSet == 1 ) && (i == 7))
    {
      setbit(*warn_flag, eBack);//不完全
      //print_dbg("warn_flag_xxvx eBack [0x%02x]",*warn_flag);
      b_tmp = 0;
    }
    else if((b_bSet == 0 ) && (i == 7))
    {
      clrbit(*warn_flag, eBack);//安全清零
      //print_dbg("warn_flag_xxxx eBack [0x%02x]",*warn_flag);
      b_tmp = 0;
    }
  }
  for(i=8;i<11;i++)
  {
    warn[eLetf][i-8] = flag_ultsafly[i];
    l_tmp |= safly_flag[i];
    l_bSet = l_tmp > 0 ? 1 : 0;
    if ((l_bSet == 1 ) && (i == 10))
    {
      setbit(*warn_flag, eLetf);//不完全
      //print_dbg("warn_flag_xvxx eLetf [0x%02x]",*warn_flag);
      l_tmp = 0;
    }
    else if((l_bSet == 0 ) && (i == 10))
    {
      clrbit(*warn_flag, eLetf);//安全清零
      //print_dbg("warn_flag_xxxx eLetf [0x%02x]",*warn_flag);
      l_tmp = 0;
    }
  for(i=11;i<14;i++)
  {
    warn[eRight][i-11] = flag_ultsafty[i];
    r_tmp |= flag_ultsafty[i];
    r_bSet = r_tmp > 0 ? 1 : 0;
    if ((r_bSet == 1 ) && (i == 13))
    {
      setbit(*warn_flag, eRight);//不完全
      //print_dbg("warn_flag_vxxx eRight [0x%02x]",*warn_flag);
      r_tmp = 0;
    }
    else if((r_bSet ==0 ) && (i==13))
    {
      clrbit(*warn_flag, eRight);//安全清零
      //print_dbg("warn_flag_xxxx eRight [0x%02x]",*warn_flag);
      r_tmp = 0;
    }
  }
}
*/


}
}  // namespace guardian
}  // namespace apollo
