/********************************************************************************
 * Copyright (c) 2016 ITK Engineering GmbH
 *               2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "agentBlueprint.h"
#include <assert.h>

AgentBlueprint::AgentBlueprint()
{
}

void AgentBlueprint::SetVehicleComponentProfileNames(VehicleComponentProfileNames vehicleComponentProfileNames)
{
    this->vehicleComponentProfileNames = vehicleComponentProfileNames;
}

void AgentBlueprint::SetAgentCategory(AgentCategory agentCategory)
{
    this->agentCategory = agentCategory;
}

void AgentBlueprint::SetAgentProfileName(std::string agentTypeName)
{
    this->agentProfileName = agentTypeName;
}

void AgentBlueprint::SetVehicleProfileName(std::string vehicleProfileName)
{
    this->vehicleProfileName = vehicleProfileName;
}

void AgentBlueprint::SetVehicleModelName(std::string vehicleModelName)
{
    this->vehicleModelName = vehicleModelName;
}

void AgentBlueprint::SetVehicleModelParameters(VehicleModelParameters vehicleModelParameters)
{
    this->vehicleModelParameters = vehicleModelParameters;
}

void AgentBlueprint::SetDriverProfileName(std::string driverProfileName)
{
    this->driverProfileName = driverProfileName;
}

void AgentBlueprint::SetSpawnParameter(SpawnParameter spawnParameter)
{
    this->spawnParameter = spawnParameter;
}

void AgentBlueprint::SetSpeedGoalMin(double speedGoalMin)
{
    this->speedGoalMin = speedGoalMin;
}

void AgentBlueprint::SetObjectName(std::string objectName)
{
    this->objectName = objectName;
}

void AgentBlueprint::AddSensor(openpass::sensors::Parameter parameters)
{
    sensorParameters.push_back(parameters);
}

AgentCategory AgentBlueprint::GetAgentCategory() const
{
    return agentCategory;
}

std::string AgentBlueprint::GetAgentProfileName() const
{
    return agentProfileName;
}

std::string AgentBlueprint::GetVehicleProfileName() const
{
    return vehicleProfileName;
}

std::string AgentBlueprint::GetVehicleModelName() const
{
    return vehicleModelName;
}

std::string AgentBlueprint::GetObjectName() const
{
    return objectName;
}

std::string AgentBlueprint::GetDriverProfileName() const
{
    return driverProfileName;
}

VehicleModelParameters AgentBlueprint::GetVehicleModelParameters() const
{
    return vehicleModelParameters;
}

openpass::sensors::Parameters AgentBlueprint::GetSensorParameters() const
{
    return sensorParameters;
}

VehicleComponentProfileNames AgentBlueprint::GetVehicleComponentProfileNames() const
{
    return vehicleComponentProfileNames;
}

void AgentBlueprint::SetAgentType(std::shared_ptr<core::AgentTypeInterface> agentType)
{
    this->agentType = agentType;
}


core::AgentTypeInterface& AgentBlueprint::GetAgentType() const
{
    assert(agentType.get());
    return *agentType.get();
}

SpawnParameter& AgentBlueprint::GetSpawnParameter()
{
    return spawnParameter;
}

const SpawnParameter& AgentBlueprint::GetSpawnParameter() const
{
    return spawnParameter;
}

double AgentBlueprint::GetSpeedGoalMin() const
{
    return speedGoalMin;
}
