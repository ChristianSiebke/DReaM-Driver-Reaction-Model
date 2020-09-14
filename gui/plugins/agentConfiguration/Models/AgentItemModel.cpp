/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "AgentItemModel.h"

#include "openPASS-AgentConfiguration/AgentMapInterface.h"

AgentItemModel::AgentItemModel(QObject *parent) :
    AgentItemInterface(parent), vehicleProfiles(new VehicleProfiles), driverProfiles(new DriverProfiles), type("Dynamic"), systemConfig(""), system(System()), vehicleModelCatalogue("")
{
}

AgentItemModel::AgentItemModel(AgentItemInterface const *const agent, QObject *parent) :
    AgentItemInterface(parent), vehicleProfiles(new VehicleProfiles), driverProfiles(new DriverProfiles), type(agent->getType()), systemConfig(agent->getSystemConfig()), system(System(agent->getSystemID(), agent->getSystemTitle())), vehicleModelCatalogue(agent->getVehicleModelCatalogue()), vehicleModel(agent->getVehicleModel())
{
    for (auto profile : agent->getVehicleProfiles()->keys())
        vehicleProfiles->insert(profile, agent->getVehicleProfiles()->value(profile));

    for (auto profile : agent->getDriverProfiles()->keys())
        driverProfiles->insert(profile, agent->getDriverProfiles()->value(profile));
}

AgentItemModel::~AgentItemModel()
{
    delete vehicleProfiles;
    delete driverProfiles;
}

AgentItemInterface::Name AgentItemModel::getName() const
{
    AgentMapInterface const *const agents = qobject_cast<AgentMapInterface const *const>(parent());

    return agents->getName(const_cast<AgentItemModel *>(this));
}

AgentItemInterface::Type AgentItemModel::getType() const
{
    return type;
}

void AgentItemModel::setType(const Type &_type)
{
    type = _type;
}

AgentItemInterface::VehicleProfiles *AgentItemModel::getVehicleProfiles() const
{
    return vehicleProfiles;
}

AgentItemInterface::DriverProfiles *AgentItemModel::getDriverProfiles() const
{
    return driverProfiles;
}

AgentItemInterface::SystemID AgentItemModel::getSystemID() const
{
    return system.first;
}

void AgentItemModel::setSystemID(SystemID const &_systemID)
{
    system.first = _systemID;
}

void AgentItemModel::setSystemTitle(const SystemTitle &_systemTitle)
{
    system.second = _systemTitle;
}

AgentItemInterface::SystemTitle AgentItemModel::getSystemTitle() const
{
    return system.second;
}

AgentItemInterface::SystemConfig AgentItemModel::getSystemConfig() const
{
    return systemConfig;
}

void AgentItemModel::setSystemConfig(SystemConfig const &_systemConfig)
{
    systemConfig = _systemConfig;
}

AgentItemInterface::System *AgentItemModel::getSystem()
{
    return &system;
}

AgentItemInterface::VehicleModelCatalogue AgentItemModel::getVehicleModelCatalogue() const
{
    return vehicleModelCatalogue;
}

AgentItemInterface::VehicleModel AgentItemModel::getVehicleModel() const
{
    return vehicleModel;
}

void AgentItemModel::setVehicleModelCatalogue(const VehicleModelCatalogue &_vehicleModelCatalogue)
{
    vehicleModelCatalogue = _vehicleModelCatalogue;
}

void AgentItemModel::setVehicleModel(const VehicleModel &_vehicleModel)
{
    vehicleModel = _vehicleModel;
}
