/****************************************************************************** 
* Copyright (c) 2017 Volkswagen Group of America. 
* 
* This program and the accompanying materials are made 
* available under the terms of the Eclipse Public License 2.0 
* which is available at https://www.eclipse.org/legal/epl-2.0/ 
* 
* SPDX-License-Identifier: EPL-2.0 
******************************************************************************/ 

#include "SubsystemOutputMapModel.h"

#include "SubsystemOutputItemModel.h"

#include "openPASS-System/SubsystemItemInterface.h"


SubsystemOutputMapModel::SubsystemOutputMapModel(ComponentOutputMapInterface * const componentOutputs, QObject * const parent)
    : SubsystemOutputMapInterface(parent)
    , componentOutputs(componentOutputs)
{
}

bool SubsystemOutputMapModel::add(SubsystemOutputMapInterface::ID const & id)
{
    bool success;
    if (!contains(id))
    {
        success = componentOutputs->add(id);

        SubsystemOutputItemModel *outputItem = new SubsystemOutputItemModel(componentOutputs->getItem(id),this);

        map.insert(id, outputItem);
        Q_EMIT added(outputItem);
        return true && success;
    }
    return false;
}

bool SubsystemOutputMapModel::add(SubsystemOutputMapInterface::ID const & id,
                                  SubsystemOutputMapInterface::Item * const item)
{
    if (!contains(id))
    {
        componentOutputs->add(id, item->getComponentOutput());
        map.insert(id, item);
        item->setParent(this);
        Q_EMIT added(item);
        return true;
    }
    return false;
}

SubsystemOutputMapInterface::Iterator SubsystemOutputMapModel::begin()
{
    return map.begin();
}

SubsystemOutputMapInterface::ConstIterator SubsystemOutputMapModel::begin() const
{
    return map.begin();
}

bool SubsystemOutputMapModel::clear()
{
    while (map.count() > 0)
    {
        delete map.take(map.keys().first());
    }
    componentOutputs->clear();
    Q_EMIT cleared();
    return true;
}

int SubsystemOutputMapModel::count() const
{
    return map.count();
}

bool SubsystemOutputMapModel::contains(SubsystemOutputMapInterface::ID const & id) const
{
    return map.keys().contains(id);
}

bool SubsystemOutputMapModel::contains(SubsystemOutputMapInterface::Item * const item) const
{
    return map.values().contains(item);
}

bool SubsystemOutputMapModel::contains(SubsystemOutputMapInterface::Index const & index) const
{
    return ((0 <= index) && (index < map.count()));
}

SubsystemOutputMapInterface::Iterator SubsystemOutputMapModel::end()
{
    return map.end();
}

SubsystemOutputMapInterface::ConstIterator SubsystemOutputMapModel::end() const
{
    return map.end();
}

bool SubsystemOutputMapModel::setID(SubsystemOutputMapInterface::Item * const item,
                                    SubsystemOutputMapInterface::ID const & id)
{
    if ((contains(item)) && (!contains(id)))
    {
        componentOutputs->setID(item->getComponentOutput(),id);
        map.insert(id, map.take(item->getID()));
        Q_EMIT modifiedID();
        return true;
    }
    return false;
}

SubsystemOutputMapInterface::ID SubsystemOutputMapModel::generateID() const
{
    SubsystemOutputMapInterface::ID id = 0;
    for (SubsystemOutputMapInterface::ID key : map.keys())
    {
        if (id <= key)
        {
            id = key + 1;
        }
    }
    return id;
}

SubsystemOutputMapInterface::ID SubsystemOutputMapModel::getID(SubsystemOutputMapInterface::Item * const item) const
{
    return map.key(item);
}

SubsystemOutputMapInterface::ID SubsystemOutputMapModel::getID(SubsystemOutputMapInterface::Index const & index) const
{
    return ((contains(index)) ? map.keys().value(index) : SubsystemOutputMapInterface::ID());
}

SubsystemOutputMapInterface::Index SubsystemOutputMapModel::getIndex(SubsystemOutputMapInterface::ID const & id) const
{
    return map.keys().indexOf(id);
}

SubsystemOutputMapInterface::Index SubsystemOutputMapModel::getIndex(SubsystemOutputMapInterface::Item * const item) const
{
    return map.values().indexOf(item);
}

SubsystemOutputMapInterface::Item * SubsystemOutputMapModel::getItem(SubsystemOutputMapInterface::ID const & id) const
{
    return map.value(id, nullptr);
}

SubsystemOutputMapInterface::Item * SubsystemOutputMapModel::getItem(SubsystemOutputMapInterface::Index const & index) const
{
    return ((contains(index)) ? map.values().value(index, nullptr) : nullptr);
}

bool SubsystemOutputMapModel::remove(SubsystemOutputMapInterface::ID const & id)
{
    if (map.contains(id))
    {
        Q_EMIT removed(map.value(id));

        // trigger signal to remove connections in which output port is involved
        Q_EMIT map.value(id)->removeConnection(map.value(id));

        componentOutputs->remove(id);
        delete map.take(id);
        return true;
    }
    return false;
}

bool SubsystemOutputMapModel::remove(SubsystemOutputMapInterface::Item * const item)
{
    return ((contains(item)) ? remove(getID(item)) : false);
}

bool SubsystemOutputMapModel::remove(SubsystemOutputMapInterface::Index const & index)
{
    return ((contains(index)) ? remove(getID(index)) : false);
}
