/******************************************************************************
 * Copyright 2017 The Apollo Authors. All Rights Reserved.
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
#include "modules/control/controller/speed_controller.h"

#include <cstdio>
#include <utility>

#include "modules/common/adapters/adapter_manager.h"
#include "modules/common/configs/vehicle_config_helper.h"
#include "modules/common/log.h"
#include "modules/common/math/math_utils.h"
#include "modules/common/time/time.h"
#include "modules/common/util/string_util.h"
#include "modules/control/common/control_gflags.h"
#include "modules/localization/common/localization_gflags.h"

namespace apollo {
namespace control {

using apollo::common::adapter::AdapterManager;
using apollo::common::ErrorCode;
using apollo::common::Status;
using apollo::common::TrajectoryPoint;
using apollo::common::VehicleStateProvider;
using apollo::common::time::Clock;
using Matrix = Eigen::MatrixXd;

SpeedController::SpeedController()
    : name_(ControlConf_ControllerType_Name(ControlConf::SPEED_CONTROLLER)) {

}

void SpeedController::CloseLogFile() {

}
void SpeedController::Stop() { CloseLogFile(); }

SpeedController::~SpeedController() { CloseLogFile(); }

Status SpeedController::Init(const ControlConf *control_conf) {
  control_conf_ = control_conf;
 
 //读取protobuf中的配置参数
 const SpeedControllerConf &speed_controller_conf =
      control_conf_->speed_controller_conf();
  //读取PID控制参数
  station_pid_controller_.Init(speed_controller_conf.station_speedpid_conf());
  Angle_pid_controller_.Init(speed_controller_conf.angle_speedpid_conf());
  //误差矩阵初始化
  matrix_error_ = Matrix::Zero(6, 1);
  return Status::OK();
}


Status SpeedController::ComputeControlCommand(
    const localization::LocalizationEstimate *localization,
    const canbus::Chassis *chassis,
    const planning::ADCTrajectory *planning_published_trajectory,
    control::ControlCommand *cmd) {

    VehicleStateProvider::instance()->set_linear_velocity(chassis->speed_mps());

  //读取目标轨迹信息
  speedController_trajectory_analyzer_ =
      std::move(TrajectoryAnalyzer(planning_published_trajectory));
  
  const SpeedControllerConf &speed_controller_conf =
      control_conf_->speed_controller_conf();

  SimpleMPCDebug *debug = cmd->mutable_debug()->mutable_simple_mpc_debug();
  debug->Clear();

  double ts = speed_controller_conf.ts();
  //double preview_time;// = lon_controller_conf.preview_window() * ts;
  //计算纵向控制偏差
  ComputeLongitudinalErrors(&speedController_trajectory_analyzer_, debug);

  //计算横向控制偏差数据
  UpdateStateAnalyticalMatching(debug);

//  matrix_error_(0, 0) = debug->lateral_error();
//  matrix_error_(1, 0) = debug->lateral_error_rate();
//  matrix_error_(2, 0) = debug->heading_error();
//  matrix_error_(3, 0) = debug->heading_error_rate();
//  matrix_error_(4, 0) = debug->station_error();
//  matrix_error_(5, 0) = debug->speed_error();
  double speed_controller_inputLinearSpeedValue_limit  = speed_controller_conf.input_linearspeed_limit();
  double speed_controller_inputAngularSpeedValue_limit  = speed_controller_conf.input_angularspeed_limit();

  double speed_controller_outputLinearSpeedValue_limit = speed_controller_conf.output_linearspeed_limit();
  double speed_controller_outputAngularSpeedValue_limit = speed_controller_conf.output_angularspeed_limit();
  
  double speedControl_weight = speed_controller_conf.controlweight();
  //横向位移偏差值
  double lateral_error_limited = matrix_error_(0, 0);
  
  //朝向偏差控制值
  double heading_error_limited = matrix_error_(2, 0);

  //纵向位置偏差控制值
  double station_error_limited = matrix_error_(4, 0);
  
  
  if(station_error_limited > speed_controller_inputLinearSpeedValue_limit)
  {
    station_error_limited = speed_controller_inputLinearSpeedValue_limit;
  }
  else if(station_error_limited < -speed_controller_inputLinearSpeedValue_limit)
  {
    station_error_limited = -speed_controller_inputLinearSpeedValue_limit;
  }

  double Angle_error_limited = 0;
  static double K_a,K_b;
  K_a = speedControl_weight;

  K_b = 1 - K_a;
  Angle_error_limited = lateral_error_limited * K_a + heading_error_limited * K_b;
  

  if(Angle_error_limited > speed_controller_inputAngularSpeedValue_limit)
  {
    Angle_error_limited = speed_controller_inputAngularSpeedValue_limit;
  }
  else if(Angle_error_limited < -speed_controller_inputAngularSpeedValue_limit)
  {
    Angle_error_limited = -speed_controller_inputAngularSpeedValue_limit;
  }

  //计算线速度控制值
  double speed_offset =
      station_pid_controller_.Control(station_error_limited, ts);
  

  linear_velocity = debug->speed_reference() + 0.7;//speed_offset + 0.3;//debug->speed_reference();  + 0.7
  //linear_velocity = speed_offset;

  if(linear_velocity > speed_controller_outputLinearSpeedValue_limit)
  {
      linear_velocity = speed_controller_outputLinearSpeedValue_limit;
  }
  else if(linear_velocity < -speed_controller_outputLinearSpeedValue_limit)
  {
      linear_velocity = -speed_controller_outputLinearSpeedValue_limit;
  }

  //计算角速度控制值
  double angle_offset =
      Angle_pid_controller_.Control(Angle_error_limited, ts);

  angular_velocity = angle_offset;  
  //SimpleMPCDebug *debug = cmd->mutable_debug()->mutable_simple_mpc_debug();

  if(angular_velocity > speed_controller_outputAngularSpeedValue_limit)
  {
    angular_velocity = speed_controller_outputAngularSpeedValue_limit;
  }
  else if(angular_velocity < -speed_controller_outputAngularSpeedValue_limit)
  {
    angular_velocity = -speed_controller_outputAngularSpeedValue_limit;
  }

  cmd->set_linear_velocity(linear_velocity);
  cmd->set_angular_velocity(angular_velocity);
  
  debug->set_lateral_error_rate(10.0);
  debug->set_heading_error_rate(1.0);

  cmd->set_speed(50);
  cmd->set_steering_rate(18);
  cmd->set_throttle(0.1324);
  cmd->set_brake(13);
  
  return Status::OK();
}

Status SpeedController::Reset() {
  station_pid_controller_.Reset();
  Angle_pid_controller_.Reset();
  return Status::OK();
}

std::string SpeedController::Name() const { return name_; }

void SpeedController::ComputeLateralErrors(
    const double x, const double y, const double theta, const double linear_v,
    const double angular_v, const TrajectoryAnalyzer &trajectory_analyzer,
    SimpleMPCDebug *debug) {
  const auto matched_point =
      trajectory_analyzer.QueryNearestPointByPosition(x, y);

  const double dx = x - matched_point.path_point().x();
  const double dy = y - matched_point.path_point().y();
  
  AINFO<<"goal_point_x= "<<matched_point.path_point().x();
  AINFO<<"present_point_x= "<<x;
  
  AINFO<<"goal_point_y= "<<matched_point.path_point().y();
  AINFO<<"present_point_y= "<<y;

  const double cos_matched_theta = std::cos(matched_point.path_point().theta());
  const double sin_matched_theta = std::sin(matched_point.path_point().theta());
  // d_error = cos_matched_theta * dy - sin_matched_theta * dx;
  debug->set_lateral_error(cos_matched_theta * dy - sin_matched_theta * dx);

  float test_value = cos_matched_theta * dy - sin_matched_theta * dx;
  AINFO<<"LateralErrors= "<<test_value;

  const double delta_theta =
      common::math::NormalizeAngle(theta - matched_point.path_point().theta());
  const double sin_delta_theta = std::sin(delta_theta);
  // d_error_dot = chassis_v * sin_delta_theta;
  debug->set_lateral_error_rate(linear_v * sin_delta_theta);

  // theta_error = delta_theta;
  debug->set_heading_error(delta_theta);

  AINFO<<"heading_error= "<<delta_theta;

  // theta_error_dot = angular_v - matched_point.path_point().kappa() *
  // matched_point.v();
  debug->set_heading_error_rate(angular_v - matched_point.path_point().kappa() *
                                                matched_point.v());

  // matched_theta = matched_point.path_point().theta();
  debug->set_ref_heading(matched_point.path_point().theta());
  // matched_kappa = matched_point.path_point().kappa();
  debug->set_curvature(matched_point.path_point().kappa());
  
}

void SpeedController::ComputeLongitudinalErrors(
    const TrajectoryAnalyzer *trajectory_analyzer,
    SimpleMPCDebug *debug) {
  // the decomposed vehicle motion onto Frenet frame
  // s: longitudinal accumulated distance along reference trajectory
  // s_dot: longitudinal velocity along reference trajectory
  // d: lateral distance w.r.t. reference trajectory
  // d_dot: lateral distance change rate, i.e. dd/dt
  double s_matched = 0.0;
  double s_dot_matched = 0.0;
  double d_matched = 0.0;
  double d_dot_matched = 0.0;

  const auto &lin_position = AdapterManager::GetLocalization()->GetLatestObserved();
  const auto &pose = lin_position.pose();

  double lin_x = pose.position().x();
  double lin_y = pose.position().y();
  double lin_heading = pose.heading();
  if (FLAGS_use_navigation_mode) 
  {
      lin_x = 0;
      lin_y = 0;
      lin_heading = 0;
  } 
  /* const auto matched_point = trajectory_analyzer->QueryMatchedPathPoint(
      VehicleStateProvider::instance()->x(),
      VehicleStateProvider::instance()->y()); */

  const auto matched_point = trajectory_analyzer->QueryMatchedPathPoint(lin_x,lin_y);

  /* trajectory_analyzer->ToTrajectoryFrame(
      VehicleStateProvider::instance()->x(),
      VehicleStateProvider::instance()->y(),
      VehicleStateProvider::instance()->heading(),
      VehicleStateProvider::instance()->linear_velocity(), matched_point,
      &s_matched, &s_dot_matched, &d_matched, &d_dot_matched); */

  trajectory_analyzer->ToTrajectoryFrame(
      lin_x,lin_y,lin_heading,
      VehicleStateProvider::instance()->linear_velocity(), matched_point,
      &s_matched, &s_dot_matched, &d_matched, &d_dot_matched);

  const double current_control_time = Clock::NowInSeconds();

  TrajectoryPoint reference_point =
      trajectory_analyzer->QueryNearestPointByAbsoluteTime(
          current_control_time);

  ADEBUG << "matched point:" << matched_point.DebugString();
  ADEBUG << "reference point:" << reference_point.DebugString();
  debug->set_station_error(reference_point.path_point().s() - s_matched);

  float test_station_error = reference_point.path_point().s() - s_matched;

  AINFO<<"reference_station= "<<reference_point.path_point().s();
  AINFO<<"curent_station= "<<s_matched;
  AINFO<<"station_error= "<<test_station_error;

  debug->set_speed_error(reference_point.v() - s_dot_matched);

  debug->set_station_reference(reference_point.path_point().s());
  debug->set_speed_reference(reference_point.v());
  debug->set_acceleration_reference(reference_point.a());

  debug->set_station_feedback(s_matched);
  debug->set_speed_feedback(
      VehicleStateProvider::instance()->linear_velocity());
}

void SpeedController::UpdateStateAnalyticalMatching(SimpleMPCDebug *debug) {
  const auto &lin_position = AdapterManager::GetLocalization()->GetLatestObserved();
  const auto &pose = lin_position.pose();
  double lin_x = pose.position().x();
  double lin_y = pose.position().y();
  double lin_heading = pose.heading();
  const auto &com = VehicleStateProvider::instance()->ComputeCOMPosition(0.55);
  if (FLAGS_use_navigation_mode) 
    {
        lin_x = 0;
        lin_y = 0;
        lin_heading = 0;
    } 
  AINFO<<"FLAGS_use_navigation_mode= "<<FLAGS_use_navigation_mode;
  if (FLAGS_use_navigation_mode) {
    ComputeLateralErrors(0.0, 0.0, lin_heading,
                         VehicleStateProvider::instance()->linear_velocity(),
                         VehicleStateProvider::instance()->angular_velocity(),
                         speedController_trajectory_analyzer_, debug);
  } else {
    ComputeLateralErrors(lin_x, lin_y,lin_heading,
                       VehicleStateProvider::instance()->linear_velocity(),
                       VehicleStateProvider::instance()->angular_velocity(),
                       speedController_trajectory_analyzer_, debug);
  }
  /* ComputeLateralErrors(com.x(), com.y(),
                       VehicleStateProvider::instance()->heading(),
                       VehicleStateProvider::instance()->linear_velocity(),
                       VehicleStateProvider::instance()->angular_velocity(),
                       speedController_trajectory_analyzer_, debug); */
    // ComputeLateralErrors(lin_x, lin_y,lin_heading,
    //                    VehicleStateProvider::instance()->linear_velocity(),
    //                    VehicleStateProvider::instance()->angular_velocity(),
    //                    speedController_trajectory_analyzer_, debug);
  // State matrix update;
  matrix_error_(0, 0) = debug->lateral_error();
  matrix_error_(1, 0) = debug->lateral_error_rate();
  matrix_error_(2, 0) = debug->heading_error();
  matrix_error_(3, 0) = debug->heading_error_rate();
  matrix_error_(4, 0) = debug->station_error();
  matrix_error_(5, 0) = debug->speed_error(); 
}

}  // namespace control
}  // namespace apollo
