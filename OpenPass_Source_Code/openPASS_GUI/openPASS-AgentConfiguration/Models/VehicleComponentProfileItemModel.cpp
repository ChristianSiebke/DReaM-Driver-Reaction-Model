/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "VehicleComponentProfileItemModel.h"
#include "ParameterItemModel.h"

#include "openPASS-AgentConfiguration/VehicleComponentProfileMapInterface.h"

#include "ParameterMapModel.h"


VehicleComponentProfileItemModel::VehicleComponentProfileItemModel(QObject *parent)
    : VehicleComponentProfileItemInterface(parent)
    , parameters(new ParameterMapModel(this))
{

}

VehicleComponentProfileItemInterface::Name VehicleComponentProfileItemModel::getName() const
{
    VehicleComponentProfileMapInterface const * const map = qobject_cast<VehicleComponentProfileMapInterface const * const > ( parent() );

    return ( (map) ? map->getID( const_cast<VehicleComponentProfileItemModel *> (this) ).first : VehicleComponentProfileItemInterface::Name() );
}


VehicleComponentProfileItemInterface::Type VehicleComponentProfileItemModel::getType() const
{
    VehicleComponentProfileMapInterface const * const map = qobject_cast<VehicleComponentProfileMapInterface const * const > ( parent() );

    return ( (map) ? map->getID( const_cast<VehicleComponentProfileItemModel *> (this) ).second : VehicleComponentProfileItemInterface::Type() );
}


VehicleComponentProfileItemInterface::Parameters * VehicleComponentProfileItemModel::getParameters() const
{
    return parameters;
}
