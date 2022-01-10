/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "VehicleProfileItemModel.h"

#include "Models/SensorMapModel.h"
#include "Models/VehicleComponentMapModel.h"
#include "openPASS-AgentConfiguration/VehicleProfileMapInterface.h"

VehicleProfileItemModel::VehicleProfileItemModel(QObject *parent) :
    VehicleProfileItemInterface(parent), modelCatalogue(""), model(""), systemConfig(""), systemProfiles(new SystemProfiles), components(new VehicleComponentMapModel(this)), sensors(new SensorMapModel(this))
{
}

VehicleProfileItemModel::VehicleProfileItemModel(VehicleProfileItemInterface const *const vehicle, QObject *parent) :
    VehicleProfileItemInterface(parent), modelCatalogue(vehicle->getModelCatalogue()), model(vehicle->getModel()), systemConfig(vehicle->getSystemConfig()), systemProfiles(new SystemProfiles), components(new VehicleComponentMapModel(this)), sensors(new SensorMapModel(this))
{
    // copy all properties from vehicle
    for (auto algoType : vehicle->getSystemProfiles()->keys())
    {
        Systems *systems = new Systems;
        for (auto system : vehicle->getSystems(algoType)->keys())
            systems->insert(system, vehicle->getSystems(algoType)->value(system));

        systemProfiles->insert(algoType, systems);
    }

    // Note: components and sensors do not need to be copied because they are not edited by the GUI. They are automatically
    // constructed from the objects attributes when the GUI's configuration is exported to a Profiles Catalogue.
}

VehicleProfileItemModel::~VehicleProfileItemModel()
{
    qDeleteAll(systemProfiles->values());
    delete systemProfiles;
}

VehicleProfileItemInterface::Name VehicleProfileItemModel::getName() const
{
    VehicleProfileMapInterface const *const map = qobject_cast<VehicleProfileMapInterface const *const>(parent());

    return ((map) ? map->getName(const_cast<VehicleProfileItemModel *>(this)) : VehicleProfileItemInterface::Name());
}

VehicleProfileItemInterface::Model VehicleProfileItemModel::getModel() const
{
    return model;
}

void VehicleProfileItemModel::setModel(Model const &_model)
{
    model = _model;
}

VehicleProfileItemInterface::ModelCatalogue VehicleProfileItemModel::getModelCatalogue() const
{
    return modelCatalogue;
}

void VehicleProfileItemModel::setModelCatalogue(ModelCatalogue const &_modelCatalogue)
{
    modelCatalogue = _modelCatalogue;
}

VehicleProfileItemModel::SystemConfig VehicleProfileItemModel::getSystemConfig() const
{
    return systemConfig;
}

void VehicleProfileItemModel::setSystemConfig(const SystemConfig &_systemConfig)
{
    systemConfig = _systemConfig;
}

VehicleProfileItemModel::SystemProfiles *VehicleProfileItemModel::getSystemProfiles() const
{
    return systemProfiles;
}

VehicleProfileItemModel::Systems *VehicleProfileItemModel::getSystems(const AlgoType &_type) const
{
    return systemProfiles->value(_type);
}

/************************************ Interferes with SystemConfig *******************************************/

VehicleProfileItemInterface::VehicleComponents *VehicleProfileItemModel::getVehicleComponents() const
{
    return components;
}

VehicleProfileItemInterface::Sensors *VehicleProfileItemModel::getSensors() const
{
    return sensors;
}
