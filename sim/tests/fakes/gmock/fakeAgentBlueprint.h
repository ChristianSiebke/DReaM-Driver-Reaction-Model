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
#include "include/agentBlueprintInterface.h"

class FakeAgentBlueprint : public AgentBlueprintInterface {
 public:
  MOCK_METHOD1(SetVehicleComponentProfileNames,
      void(VehicleComponentProfileNames));
  MOCK_METHOD1(SetAgentCategory,
      void(AgentCategory agentTypeGroupName));
  MOCK_METHOD1(SetAgentProfileName,
      void(std::string agentTypeName));
  MOCK_METHOD1(SetVehicleProfileName,
      void(std::string vehicleModelName));
  MOCK_METHOD1(SetVehicleModelName,
      void(std::string vehicleModelName));
  MOCK_METHOD1(SetVehicleModelParameters,
      void(VehicleModelParameters vehicleModelParameters));
  MOCK_METHOD1(SetDriverProfileName,
      void(std::string));
  MOCK_METHOD1(SetSpawnParameter,
      void(SpawnParameter spawnParameter));
  MOCK_METHOD1(SetSpeedGoalMin,
      void(double));
  MOCK_CONST_METHOD0(GetAgentCategory,
      AgentCategory());
  MOCK_CONST_METHOD0(GetAgentProfileName,
      std::string());
  MOCK_CONST_METHOD0(GetVehicleProfileName,
      std::string());
  MOCK_CONST_METHOD0(GetVehicleModelName,
      std::string());
  MOCK_CONST_METHOD0(GetVehicleModelParameters,
      VehicleModelParameters());
  MOCK_CONST_METHOD0(GetVehicleComponentProfileNames,
      VehicleComponentProfileNames());
  MOCK_CONST_METHOD0(GetDriverProfileName,
      std::string());
  MOCK_CONST_METHOD0(GetObjectName,
      std::string());
  MOCK_CONST_METHOD0(GetAgentType,
      core::AgentTypeInterface&());
  MOCK_METHOD0(GetSpawnParameter,
      SpawnParameter&());
  MOCK_CONST_METHOD0(GetSpawnParameter,
      const SpawnParameter&());
  MOCK_CONST_METHOD0(GetSpeedGoalMin,
      double());
  MOCK_METHOD1(SetObjectName,
      void(std::string));
  MOCK_METHOD1(AddSensor,
      void(openpass::sensors::Parameter));
  MOCK_CONST_METHOD0(GetSensorParameters,
      openpass::sensors::Parameters());
  MOCK_METHOD1(SetAgentType,
      void (std::shared_ptr<core::AgentTypeInterface> agentType));
};
