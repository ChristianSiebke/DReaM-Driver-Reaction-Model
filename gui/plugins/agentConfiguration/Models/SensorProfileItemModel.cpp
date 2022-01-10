/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "SensorProfileItemModel.h"

#include "ParameterMapModel.h"
#include "openPASS-AgentConfiguration/SensorProfileMapInterface.h"

SensorProfileItemModel::SensorProfileItemModel(QObject *parent) :
    SensorProfileItemInterface(parent), parameters(new ParameterMapModel(this))
{
}

SensorProfileItemInterface::Name SensorProfileItemModel::getName() const
{
    SensorProfileMapInterface const *const map = qobject_cast<SensorProfileMapInterface const *const>(parent());

    return ((map) ? map->getID(const_cast<SensorProfileItemModel *>(this)).first : SensorProfileItemInterface::Name());
}

SensorProfileItemInterface::Type SensorProfileItemModel::getType() const
{
    SensorProfileMapInterface const *const map = qobject_cast<SensorProfileMapInterface const *const>(parent());

    return ((map) ? map->getID(const_cast<SensorProfileItemModel *>(this)).second : SensorProfileItemInterface::Type());
}

SensorProfileItemInterface::Parameters *SensorProfileItemModel::getParameters() const
{
    return parameters;
}

SensorProfileItemInterface::ID SensorProfileItemModel::getID() const
{
    SensorProfileMapInterface const *const map = qobject_cast<SensorProfileMapInterface const *const>(parent());

    return ((map) ? map->getID(const_cast<SensorProfileItemModel *>(this)) : SensorProfileItemInterface::ID());
}
