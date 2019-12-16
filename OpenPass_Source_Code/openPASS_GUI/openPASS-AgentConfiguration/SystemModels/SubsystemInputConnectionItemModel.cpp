/****************************************************************************** 
* Copyright (c) 2017 Volkswagen Group of America. 
* 
* This program and the accompanying materials are made 
* available under the terms of the Eclipse Public License 2.0 
* which is available at https://www.eclipse.org/legal/epl-2.0/ 
* 
* SPDX-License-Identifier: EPL-2.0 
******************************************************************************/ 

#include "SubsystemInputConnectionItemModel.h"

#include "SubsystemInputConnectionMapModel.h"

SubsystemInputConnectionItemModel::SubsystemInputConnectionItemModel(SubsystemInputConnectionItemInterface::Source const * const source,
                                                     SubsystemInputConnectionItemInterface::Target const * const target,
                                                     QObject * const parent)
    : SubsystemInputConnectionItemInterface(parent)
    , source(source)
    , target(target)
{
}

SubsystemInputConnectionItemInterface::ID SubsystemInputConnectionItemModel::getID() const
{
    SubsystemInputConnectionMapModel const * const connections =
            qobject_cast<SubsystemInputConnectionMapModel const * const>(parent());
    return ((connections) ? connections->getID(const_cast<SubsystemInputConnectionItemModel *>(this))
                          : SubsystemInputConnectionItemInterface::ID());
}

SubsystemInputConnectionItemInterface::Source const * SubsystemInputConnectionItemModel::getSource() const
{
    return source;
}

SubsystemInputConnectionItemInterface::Target const * SubsystemInputConnectionItemModel::getTarget() const
{
    return target;
}

bool SubsystemInputConnectionItemModel::isCompatible() const
{
    return isCompatible(source, target);
}

bool SubsystemInputConnectionItemModel::isCompatible(SubsystemInputConnectionItemInterface::Source const * const source,
                                             SubsystemInputConnectionItemInterface::Target const * const target)
{

     return ((source->getType() == target->getType()) &&
            ((source->getUnit() == target->getUnit()) ||
             (source->getUnit() == QStringLiteral("")) ||
             (target->getUnit() == QStringLiteral(""))));
}
