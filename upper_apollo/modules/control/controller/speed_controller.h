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

/**
 * @file
 * @brief Defines the LonController class.
 */

#ifndef MODULES_CONTROL_CONTROLLER_SPEED_CONTROLLER_H_
#define MODULES_CONTROL_CONTROLLER_SPEED_CONTROLLER_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Eigen/Core"
#include "modules/common/configs/proto/vehicle_config.pb.h"
#include "modules/common/filters/digital_filter.h"
#include "modules/common/filters/digital_filter_coefficients.h"
#include "modules/common/vehicle_state/vehicle_state_provider.h"
#include "modules/control/common/interpolation_2d.h"
#include "modules/control/common/pid_controller.h"
#include "modules/control/common/trajectory_analyzer.h"
#include "modules/control/controller/controller.h"

/**
 * @namespace apollo::control
 * @brief apollo::control
 */
namespace apollo {
namespace control {


class SpeedController : public Controller {
 public:

  SpeedController();


  virtual ~SpeedController();


  common::Status Init(const ControlConf *control_conf) override;


  common::Status ComputeControlCommand(
      const localization::LocalizationEstimate *localization,
      const canbus::Chassis *chassis, const planning::ADCTrajectory *trajectory,
      control::ControlCommand *cmd) override;

  double linear_velocity;
  double angular_velocity;
  common::Status Reset() override;


  void Stop() override;


  std::string Name() const override;

 protected:
  void ComputeLongitudinalErrors(const TrajectoryAnalyzer *trajectory,
                                 SimpleMPCDebug *debug);

void ComputeLateralErrors(
    const double x, const double y, const double theta, const double linear_v,
    const double angular_v, const TrajectoryAnalyzer &trajectory_analyzer,
    SimpleMPCDebug *debug);
 private:

void UpdateStateAnalyticalMatching(SimpleMPCDebug *debug);

  void CloseLogFile();

  const localization::LocalizationEstimate *localization_ = nullptr;
  const canbus::Chassis *chassis_ = nullptr;

  std::unique_ptr<Interpolation2D> control_interpolation_;
  const planning::ADCTrajectory *trajectory_message_ = nullptr;
  std::unique_ptr<TrajectoryAnalyzer> trajectory_analyzer_;

  std::string name_;
  bool controller_initialized_ = false;

  PIDController station_pid_controller_;
  PIDController Angle_pid_controller_;

  FILE *speed_log_file_ = nullptr;

  // error matrix
  Eigen::MatrixXd matrix_error_;

  // a proxy to analyze the planning trajectory
  TrajectoryAnalyzer speedController_trajectory_analyzer_;

  const ControlConf *control_conf_ = nullptr;

  // vehicle parameter
  common::VehicleParam vehicle_param_;
};
}  // namespace control
}  // namespace apollo
#endif  // MODULES_CONTROL_CONTROLLER_SPEED_CONTROLLER_H_
