/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "Models/AgentConfigurationModel.h"

AgentConfigurationModel::AgentConfigurationModel(QObject *parent) :
    AgentConfigurationInterface(parent), agents(new AgentMapModel(this)), vehicleProfiles(new VehicleProfileMapModel(this)), driverProfiles(new DriverProfileMapModel(this)), componentProfiles(new VehicleComponentProfileMapModel(this)), sensorProfiles(new SensorProfileMapModel(this))
{
}

AgentMapInterface *AgentConfigurationModel::getAgents() const
{
    return agents;
}

VehicleProfileMapInterface *AgentConfigurationModel::getVehicleProfiles() const
{
    return vehicleProfiles;
}

DriverProfileMapInterface *AgentConfigurationModel::getDriverProfiles() const
{
    return driverProfiles;
}

VehicleComponentProfileMapInterface *AgentConfigurationModel::getVehicleComponentProfiles() const
{
    return componentProfiles;
}

SensorProfileMapInterface *AgentConfigurationModel::getSensorProfiles() const
{
    return sensorProfiles;
}
