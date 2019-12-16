/****************************************************************************** 
* Copyright (c) 2017 Volkswagen Group of America. 
* 
* This program and the accompanying materials are made 
* available under the terms of the Eclipse Public License 2.0 
* which is available at https://www.eclipse.org/legal/epl-2.0/ 
* 
* SPDX-License-Identifier: EPL-2.0 
******************************************************************************/ 

#include "SubsystemOutputConnectionItemModel.h"

#include "SubsystemOutputConnectionMapModel.h"

SubsystemOutputConnectionItemModel::SubsystemOutputConnectionItemModel(SubsystemOutputConnectionItemInterface::Source const * const source,
                                                     SubsystemOutputConnectionItemInterface::Target const * const target,
                                                     QObject * const parent)
    : SubsystemOutputConnectionItemInterface(parent)
    , source(source)
    , target(target)
{
}

SubsystemOutputConnectionItemInterface::ID SubsystemOutputConnectionItemModel::getID() const
{
    SubsystemOutputConnectionMapModel const * const connections =
            qobject_cast<SubsystemOutputConnectionMapModel const * const>(parent());
    return ((connections) ? connections->getID(const_cast<SubsystemOutputConnectionItemModel *>(this))
                          : SubsystemOutputConnectionItemInterface::ID());
}

SubsystemOutputConnectionItemInterface::Source const * SubsystemOutputConnectionItemModel::getSource() const
{
    return source;
}

SubsystemOutputConnectionItemInterface::Target const * SubsystemOutputConnectionItemModel::getTarget() const
{
    return target;
}

bool SubsystemOutputConnectionItemModel::isCompatible() const
{
    return isCompatible(source, target);
}

bool SubsystemOutputConnectionItemModel::isCompatible(SubsystemOutputConnectionItemInterface::Source const * const source,
                                             SubsystemOutputConnectionItemInterface::Target const * const target)
{

           return ((source->getType() == target->getType()) &&
            ((source->getUnit() == target->getUnit()) ||
             (source->getUnit() == QStringLiteral("")) ||
             (target->getUnit() == QStringLiteral(""))));
}
