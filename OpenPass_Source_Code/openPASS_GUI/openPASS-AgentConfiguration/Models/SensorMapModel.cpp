/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "SensorMapModel.h"

#include "SensorItemModel.h"

SensorMapModel::SensorMapModel(QObject *parent)
    : SensorMapInterface(parent)
{

}

SensorMapInterface::Iterator SensorMapModel::begin()
{
    return  sensors.begin();
}

SensorMapInterface::ConstIterator SensorMapModel::constBegin() const
{
    return sensors.constBegin();
}

SensorMapInterface::Iterator SensorMapModel::end()
{
    return sensors.end();
}

SensorMapInterface::ConstIterator SensorMapModel::constEnd() const
{
    return sensors.constEnd();
}


bool SensorMapModel::add(const SensorMapInterface::ID &id)
{
    if(!contains(id))
    {
       sensors.insert(id, new SensorItemModel(this));
       return true;
    }

    return false;
}

bool SensorMapModel::add(SensorMapInterface::Item * const item)
{
      SensorMapInterface::ID id = generateID();

    if(!contains(id))
    {
        sensors.insert(id, item);
        item->setParent(this);
        return true;
    }

    return false;
}

bool SensorMapModel::remove(const SensorMapInterface::ID &id)
{
    if(contains(id))
    {
        delete sensors.take(id);
        return true;
    }

    return false;
}


bool SensorMapModel::remove(SensorMapInterface::Item * const item)
{

    if(sensors.values().contains(item) )
    {
        SensorItemInterface::ID id = getID(item);
        delete sensors.take(id);
        return true;
    }

    return false;
}

SensorMapInterface::Item * SensorMapModel::getItem(SensorMapInterface::ID const & id) const
{
    if(contains(id))
        return sensors.value(id);

    return nullptr;
}


SensorMapInterface::ID SensorMapModel::getID(Item * const item) const
{
    return sensors.key(item);
}

void SensorMapModel::setID(Item * const item,
                                SensorMapInterface::ID const &id)
{

   SensorMapInterface::ID oldID = getID(item);

   sensors.take(oldID);
   sensors.insert(id, item);
}

SensorMapInterface::ID SensorMapModel::generateID()
{
    SensorMapInterface::ID id = 1;
    for (SensorMapInterface::ID key : sensors.keys())
    {
        if (id <= key)
        {
            id = key + 1;
        }
    }
    return id;
}

bool SensorMapModel::contains(const SensorMapInterface::ID &id) const
{
    return sensors.contains(id);
}

QList<SensorMapInterface::ID> SensorMapModel::keys() const
{
    return sensors.keys();
}

QList <SensorItemInterface*> SensorMapModel::values() const
{
    return sensors.values();
}

int SensorMapModel::count() const
{
    return sensors.count();
}

void SensorMapModel::clear()
{
   qDeleteAll(sensors);
   sensors.clear();
}
