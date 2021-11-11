/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "DriverProfileItemModel.h"

#include "ParameterMapModel.h"
#include "openPASS-AgentConfiguration/DriverProfileMapInterface.h"

DriverProfileItemModel::DriverProfileItemModel(QObject *parent) :
    DriverProfileItemInterface(parent), parameters(new ParameterMapModel(this)), driverConfig("")
{
}

DriverProfileItemModel::DriverProfileItemModel(DriverProfileItemInterface const *const driverProfile, QObject *parent) :
    DriverProfileItemInterface(parent), parameters(new ParameterMapModel(this)), driverConfig(driverProfile->getConfig())
{
    for (auto parameter : driverProfile->getParameters()->values())
    {
        ParameterMapInterface::ID const Id = parameter->getID();
        parameters->add(Id);
        ParameterItemInterface *const newParameter = parameters->getItem(Id);

        newParameter->setKey(parameter->getKey());
        newParameter->setType(parameter->getType());
        newParameter->setUnit(parameter->getUnit());
        newParameter->setValue(parameter->getValue());
    }
}

DriverProfileItemInterface::Name DriverProfileItemModel::getName() const
{
    DriverProfileMapInterface const *const map = qobject_cast<DriverProfileMapInterface const *const>(parent());

    return ((map) ? map->getName(const_cast<DriverProfileItemModel *>(this)) : DriverProfileItemInterface::Name());
}

DriverProfileItemInterface::Parameters *DriverProfileItemModel::getParameters() const
{
    return parameters;
}

DriverProfileItemInterface::DriverConfig DriverProfileItemModel::getConfig() const
{
    return driverConfig;
}

void DriverProfileItemModel::setConfig(DriverConfig const &_driverConfig)
{
    driverConfig = _driverConfig;
}
