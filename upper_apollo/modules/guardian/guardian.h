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

/**
 * @file
 */

#ifndef MODEULES_GUARDIAN_GUARDIAN_H_
#define MODEULES_GUARDIAN_GUARDIAN_H_

#include <map>
#include <mutex>
#include <queue>
#include <string>

#include "modules/canbus/proto/chassis.pb.h"
#include "modules/common/apollo_app.h"
#include "modules/common/macro.h"
#include "modules/control/proto/control_cmd.pb.h"
#include "modules/guardian/proto/guardian.pb.h"
#include "modules/monitor/proto/system_status.pb.h"
#include "ros/include/ros/ros.h"

/**
 * @namespace apollo::guardian
 * @brief apollo::guardian
 */
namespace apollo {
namespace guardian {

class Guardian : public apollo::common::ApolloApp {
 public:
  std::string Name() const override;
  apollo::common::Status Init() override;
  apollo::common::Status Start() override;
  void Stop() override;

 private:
  void OnTimer(const ros::TimerEvent&);
  void OnChassis(const apollo::canbus::Chassis& message);
  void OnControl(const apollo::control::ControlCommand& message);
  void OnSystemStatus(const apollo::monitor::SystemStatus& message);
  void PassThroughControlCommand();
  void TriggerSafetyMode();
 
  //wh:ult_safty
  void TriggerSafety();
  void check_obs_of_Num(int i,int *flag_ultsafty[],uint16_t *warn_flag);

  apollo::canbus::Chassis chassis_;
  apollo::monitor::SystemStatus system_status_;
  apollo::control::ControlCommand control_cmd_;
  apollo::guardian::GuardianCommand guardian_cmd_;

  std::mutex mutex_;

  ros::Timer timer_;

  // wh:ult_safty
  enum _eRUN_
  {
    eArea_safty   =0,       /*安全区域  (线角速度不超过最大速度)*/
    eArea_warn    =1,       /*警告区域  (线角速度不超过警示速度)*/
    eArea_brake   =2,       /*刹车区域 一级警告  (线速度为0)*/ 
    eArea_danger  =3,       /*刹车区域 二级级警告  (线角速度为0)*/
    eArea_dangst  =4,       /*盲区域 危险*/
  };
  enum _eRLBF_
  {
    eFront  =0,       /*front*/
    eBack   =1,       /*back*/
    eLetf   =2,       /*letf*/
    eRight  =3,       /*right*/
  };
  enum _eObstacle_
  {                       /*eObs_右 左 后 前 */
    eObs_xxxx   =0,
    eObs_xxxv   =1,       /* x x x v */    
    eObs_xxvx   =2,
    eObs_xxvv   =3,
    eObs_xvxx   =4,
    eObs_xvxv   =5,
    eObs_xvvx   =6,
    eObs_xvvv   =7,
    eObs_vxxx   =8,
    eObs_vxxv   =9,
    eObs_vxvx   =10,
    eObs_vxvv   =11,
    eObs_vvxx   =12,
    eObs_vvxv   =13,
    eObs_vvvx   =14,
    eObs_vvvv   =15,       
  };
};

}  // namespace guardian
}  // namespace apollo

#endif  // MODULES_GUARDIAN_GUARDIAN_H_
