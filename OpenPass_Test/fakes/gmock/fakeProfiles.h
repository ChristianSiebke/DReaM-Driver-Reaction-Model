/*********************************************************************
* Copyright (c) 2019 in-tech
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#pragma once

#include "gmock/gmock.h"
#include "Interfaces/profilesInterface.h"

class FakeProfiles : public ProfilesInterface {
 public:
  MOCK_METHOD0(GetAgentProfiles,
      std::unordered_map<std::string, AgentProfile> &());
  MOCK_METHOD0(GetVehicleProfiles,
      std::unordered_map<std::string, VehicleProfile> &());
  MOCK_METHOD0(GetDriverProfiles,
      DriverProfiles &());
  MOCK_METHOD0(GetVehicleComponentProfiles,
      std::unordered_map<std::string, VehicleComponentProfiles>&());
  MOCK_METHOD0(GetSensorProfiles,
      std::list<SensorProfile> &());
  MOCK_METHOD1(GetDriverProbabilities,
      StringProbabilities &(std::string));
  MOCK_METHOD1(GetVehicleProfileProbabilities,
      StringProbabilities &(std::string));
};
