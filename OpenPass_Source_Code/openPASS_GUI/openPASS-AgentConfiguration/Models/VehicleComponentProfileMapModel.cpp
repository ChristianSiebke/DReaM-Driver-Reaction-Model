/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "VehicleComponentProfileMapModel.h"

#include "VehicleComponentProfileItemModel.h"

VehicleComponentProfileMapModel::VehicleComponentProfileMapModel(QObject *parent)
    : VehicleComponentProfileMapInterface(parent)
{

}


VehicleComponentProfileMapInterface::Iterator VehicleComponentProfileMapModel::begin()
{
    return  componentProfiles.begin();
}

VehicleComponentProfileMapInterface::ConstIterator VehicleComponentProfileMapModel::constBegin() const
{
    return componentProfiles.constBegin();
}

VehicleComponentProfileMapInterface::Iterator VehicleComponentProfileMapModel::end()
{
    return componentProfiles.end();
}

VehicleComponentProfileMapInterface::ConstIterator VehicleComponentProfileMapModel::constEnd() const
{
    return componentProfiles.constEnd();
}


bool VehicleComponentProfileMapModel::add(const VehicleComponentProfileMapInterface::ID &id)
{
    if(!contains(id))
    {
       componentProfiles.insert(id, new VehicleComponentProfileItemModel(this));
       return true;
    }

    return false;
}

bool VehicleComponentProfileMapModel::remove(const VehicleComponentProfileMapInterface::ID &id)
{
    if(contains(id))
    {
        delete componentProfiles.take(id);
        return true;
    }

    return false;
}


bool VehicleComponentProfileMapModel::remove(VehicleComponentProfileMapInterface::Item * const item)
{

    if(componentProfiles.values().contains(item) )
    {
        VehicleComponentProfileMapInterface::ID id = getID(item);
        delete componentProfiles.take(id);
        return true;
    }

    return false;
}

VehicleComponentProfileMapInterface::Item * VehicleComponentProfileMapModel::getItem(VehicleComponentProfileMapInterface::ID const & id) const
{
    if(contains(id))
        return componentProfiles.value(id);

    return nullptr;
}


VehicleComponentProfileMapInterface::ID VehicleComponentProfileMapModel::getID(Item * const item) const
{
    return componentProfiles.key(item);
}

void VehicleComponentProfileMapModel::setID(Item * const item,
                                VehicleComponentProfileMapInterface::ID const &id)
{

   VehicleComponentProfileMapInterface::ID oldID = getID(item);

   componentProfiles.take(oldID);
   componentProfiles.insert(id, item);
}


bool VehicleComponentProfileMapModel::contains(const VehicleComponentProfileMapInterface::ID &id) const
{
    return componentProfiles.contains(id);
}

QList<VehicleComponentProfileMapInterface::ID> VehicleComponentProfileMapModel::keys() const
{
    return componentProfiles.keys();
}

QList <VehicleComponentProfileItemInterface*> VehicleComponentProfileMapModel::values() const
{
    return componentProfiles.values();
}

int VehicleComponentProfileMapModel::count() const
{
    return componentProfiles.count();
}

void VehicleComponentProfileMapModel::clear()
{
   qDeleteAll(componentProfiles);
   componentProfiles.clear();
}
