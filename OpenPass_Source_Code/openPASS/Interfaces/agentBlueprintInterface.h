/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*               2016 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  AgentBlueprintInterface.h
//! @brief This file contains the interface of the agent blueprint to interact
//!        with the framework.
//-----------------------------------------------------------------------------

#include <string>
#include <unordered_map>
#include "agentTypeInterface.h"
#include "profilesInterface.h"
#include "Common/globalDefinitions.h"
#include "Common/worldDefinitions.h"

#pragma once

using VehicleComponentProfileNames = std::unordered_map<std::string, std::string>;

struct SpawnParameter
{
    double positionX = -999;
    double positionY = -999;
    double velocity = -999;
    double acceleration = -999;
    double gear = -999;
    double yawAngle = -999;
    std::optional<Route> route{};
};

class AgentBlueprintInterface
{
public:
    AgentBlueprintInterface() = default;
    virtual ~AgentBlueprintInterface() = default;

    //Setter
    virtual void SetVehicleComponentProfileNames(VehicleComponentProfileNames vehicleComponentProfileName) = 0;
    virtual void SetAgentCategory(AgentCategory agentCategory) = 0;
    virtual void SetAgentProfileName(std::string agentTypeName) = 0;
    virtual void SetVehicleProfileName(std::string vehicleModelName) = 0;
    virtual void SetVehicleModelName(std::string vehicleModelName) = 0;
    virtual void SetVehicleModelParameters(VehicleModelParameters vehicleModelParameters) = 0;
    virtual void SetDriverProfileName(std::string driverProfileName) = 0;
    virtual void SetSpawnParameter(SpawnParameter spawnParameter) = 0;
    virtual void SetSpeedGoalMin(double speedGoalMin) = 0;
    virtual void SetAgentType(std::shared_ptr<SimulationSlave::AgentTypeInterface> agentType) = 0;

    virtual void AddSensor(openpass::sensors::Parameter parameters) = 0;

    virtual AgentCategory                           GetAgentCategory() const = 0;
    virtual std::string                             GetAgentProfileName() const = 0;
    virtual std::string                             GetVehicleProfileName() const = 0;
    virtual std::string                             GetVehicleModelName() const = 0;
    virtual std::string                             GetDriverProfileName() const = 0;
    virtual std::string                             GetObjectName() const = 0;

    virtual VehicleModelParameters                  GetVehicleModelParameters() const = 0;
    virtual openpass::sensors::Parameters           GetSensorParameters() const = 0;
    virtual VehicleComponentProfileNames            GetVehicleComponentProfileNames() const = 0;
    virtual SimulationSlave::AgentTypeInterface&    GetAgentType() const = 0;
    virtual SpawnParameter&                         GetSpawnParameter() = 0;
    virtual const SpawnParameter&                   GetSpawnParameter() const = 0;
    virtual double                                  GetSpeedGoalMin() const = 0;

    virtual void                                    SetObjectName(std::string objectName) = 0;
};


