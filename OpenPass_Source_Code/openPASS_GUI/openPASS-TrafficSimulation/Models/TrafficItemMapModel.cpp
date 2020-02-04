/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "TrafficItemMapModel.h"
#include "TrafficItemModel.h"


TrafficItemMapModel::TrafficItemMapModel(QObject *parent)
    :TrafficItemMapInterface(parent)
{
}

 TrafficItemMapInterface::Iterator TrafficItemMapModel::begin()
{
    return map.begin();
}


 TrafficItemMapInterface::ConstIterator TrafficItemMapModel::constBegin() const
{
    return map.constBegin();
}


 TrafficItemMapInterface::Iterator TrafficItemMapModel::end()
{
    return map.end();
}


 TrafficItemMapInterface::ConstIterator TrafficItemMapModel::constEnd() const
{
    return map.constEnd();
}


bool TrafficItemMapModel::add(const  TrafficItemMapInterface::ID &id,
                              TrafficItemInterface::Probability const & probability,
                              TrafficItemInterface::Value const & value)
{
    if(!contains(id))
    {
        TrafficItemInterface* item = new TrafficItemModel(this);
        item->setValue(value);
        item->setProbability(probability);
        map.insert(id,item);

        connect(item, &TrafficItemInterface::edited, this, &TrafficItemMapInterface::edited);

        Q_EMIT edited();
        return true;
    }

    return false;
}


bool TrafficItemMapModel::add( TrafficItemMapInterface::Item * const item)
{
     TrafficItemMapInterface::ID id = generateID();

    if(!contains(id) && !values().contains(item))
    {
        connect(item, &TrafficItemInterface::edited, this, &TrafficItemMapInterface::edited);
        map.insert(id, item);
        item->setParent(this);

        Q_EMIT edited();
        return true;
    }

    return false;
}


bool TrafficItemMapModel::remove(const  TrafficItemMapInterface::ID &id)
{
    if(contains(id))
    {
        delete map.take(id);

        Q_EMIT edited();
        return true;
    }

    return false;
}


bool TrafficItemMapModel::remove(  TrafficItemMapInterface::Item * const item)
{

    if( map.values().contains(item) )
    {
         TrafficItemMapInterface::ID id = getID(item);
        delete map.take(id);

         Q_EMIT edited();
        return true;
    }

    return false;
}


 TrafficItemMapInterface::Item * TrafficItemMapModel::getItem( TrafficItemMapInterface::ID const & id) const
{
    if(contains(id))
    {
        return map.value(id);
    }

    return nullptr;
}


 TrafficItemMapInterface::ID TrafficItemMapModel::getID( TrafficItemMapInterface::Item * const item) const
{
    return map.key(item);
}


bool TrafficItemMapModel::contains(const  TrafficItemMapInterface::ID &id) const
{
    return map.contains(id);
}


QList< TrafficItemMapInterface::ID> TrafficItemMapModel::keys() const
{
    return map.keys();
}


QList< TrafficItemMapInterface::Item*> TrafficItemMapModel::values() const
{
    return map.values();
}

 TrafficItemMapInterface::ID TrafficItemMapModel::generateID() const
{
     TrafficItemMapInterface::ID id = 0;
    for (auto key : map.keys())
    {
        if (id <= key)
        {
            id = key + 1;
        }
    }
    return id;
}


int TrafficItemMapModel::count() const
{
    return map.count();
}


void TrafficItemMapModel::clear()
{
    qDeleteAll(map);
    map.clear();
}
