/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "VehicleComponentItemModel.h"

#include "openPASS-AgentConfiguration/VehicleComponentMapInterface.h"

VehicleComponentItemModel::VehicleComponentItemModel(QObject *parent) :
    VehicleComponentItemInterface(parent), profiles(new Profiles()), sensorLinks(new SensorLinks())
{
}

VehicleComponentItemModel::~VehicleComponentItemModel()
{
    delete profiles;
    delete sensorLinks;
}

VehicleComponentItemInterface::Type VehicleComponentItemModel::getType() const
{
    VehicleComponentMapInterface const *const map = qobject_cast<VehicleComponentMapInterface const *const>(parent());

    return ((map) ? map->getType(const_cast<VehicleComponentItemModel *>(this)) : VehicleComponentItemInterface::Type());
}

VehicleComponentItemInterface::Profiles *VehicleComponentItemModel::getProfiles() const
{
    return profiles;
}

VehicleComponentItemInterface::SensorLinks *VehicleComponentItemModel::getSensorLinks() const
{
    return sensorLinks;
}
