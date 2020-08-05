/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "SensorItemModel.h"

#include "openPASS-AgentConfiguration/SensorMapInterface.h"

SensorPosition SensorItemModel::default_position ={ QString("Default"), .0, 0.0, 0.0, 0.0, 0.0, 0.0};

SensorItemModel::SensorItemModel(QObject *parent)
    : SensorItemInterface(parent)
    , position(default_position)
    , profile(nullptr)
{

}

SensorPosition SensorItemModel::getSensorPosition() const
{
    return position;
}

void SensorItemModel::setSensorPosition(SensorPosition const & _position)
{
    position = _position;
}

SensorItemInterface::Profile * SensorItemModel::getSensorProfile() const
{
    return profile;
}

void SensorItemModel::setSensorProfile(Profile * const _profile)
{
    profile=_profile;
}


SensorItemInterface::ID SensorItemModel::getID() const
{
    SensorMapInterface const * const map = qobject_cast<SensorMapInterface const * const > ( parent() );

    return ( (map) ? map->getID( const_cast<SensorItemModel *> (this) ) : SensorItemInterface::ID() );
}
