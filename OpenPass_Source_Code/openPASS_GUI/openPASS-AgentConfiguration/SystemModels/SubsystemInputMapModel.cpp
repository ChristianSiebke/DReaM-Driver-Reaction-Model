/****************************************************************************** 
* Copyright (c) 2017 Volkswagen Group of America. 
* 
* This program and the accompanying materials are made 
* available under the terms of the Eclipse Public License 2.0 
* which is available at https://www.eclipse.org/legal/epl-2.0/ 
* 
* SPDX-License-Identifier: EPL-2.0 
******************************************************************************/ 

#include "SubsystemInputMapModel.h"

#include "SubsystemInputItemModel.h"

#include "openPASS-System/SubsystemItemInterface.h"

SubsystemInputMapModel::SubsystemInputMapModel(ComponentInputMapInterface * const componentInputs, QObject * const parent)
    : SubsystemInputMapInterface(parent)
    , componentInputs(componentInputs)
{
}

bool SubsystemInputMapModel::add(SubsystemInputMapInterface::ID const & id)
{
    bool success;
    if (!contains(id))
    {
        success = componentInputs->add(id);

        SubsystemInputItemModel *inputItem = new SubsystemInputItemModel(componentInputs->getItem(id),this);
        map.insert(id,inputItem);

        Q_EMIT added(inputItem);

        return true && success;
    }
    return false;
}

bool SubsystemInputMapModel::add(SubsystemInputMapInterface::ID const & id,
                                 SubsystemInputMapInterface::Item * const item)
{
    if (!contains(id))
    {
        componentInputs->add(id,item->getComponentInput());
        map.insert(id, item);
        item->setParent(this);
        Q_EMIT added(item);
        return true;
    }
    return false;
}

SubsystemInputMapInterface::Iterator SubsystemInputMapModel::begin()
{
    return map.begin();
}

SubsystemInputMapInterface::ConstIterator SubsystemInputMapModel::begin() const
{
    return map.begin();
}

bool SubsystemInputMapModel::clear()
{
    while (map.count() > 0)
    {
        delete map.take(map.keys().first());
    }
    componentInputs->clear();
    Q_EMIT cleared();
    return true;
}

int SubsystemInputMapModel::count() const
{
    return map.count();
}

bool SubsystemInputMapModel::contains(SubsystemInputMapInterface::ID const & id) const
{
    return map.keys().contains(id);
}

bool SubsystemInputMapModel::contains(SubsystemInputMapInterface::Item * const item) const
{
    return map.values().contains(item);
}

bool SubsystemInputMapModel::contains(SubsystemInputMapInterface::Index const & index) const
{
    return ((0 <= index) && (index < map.count()));
}

SubsystemInputMapInterface::Iterator SubsystemInputMapModel::end()
{
    return map.end();
}

SubsystemInputMapInterface::ConstIterator SubsystemInputMapModel::end() const
{
    return map.end();
}

bool SubsystemInputMapModel::setID(SubsystemInputMapInterface::Item * const item,
                                   SubsystemInputMapInterface::ID const & id)
{

    if ((contains(item)) && (!contains(id)))
    {
        componentInputs->setID(item->getComponentInput(),id);
        map.insert(id, map.take(item->getID()));

        Q_EMIT modifiedID();
        return true;
    }
    return false;
}

SubsystemInputMapInterface::ID SubsystemInputMapModel::generateID() const
{
    SubsystemInputMapInterface::ID id = 0;
    for (SubsystemInputMapInterface::ID key : map.keys())
    {
        if (id <= key)
        {
            id = key + 1;
        }
    }
    return id;
}

SubsystemInputMapInterface::ID SubsystemInputMapModel::getID(SubsystemInputMapInterface::Item * const item) const
{
    return map.key(item);
}

SubsystemInputMapInterface::ID SubsystemInputMapModel::getID(SubsystemInputMapInterface::Index const & index) const
{
    return ((contains(index)) ? map.keys().value(index) : SubsystemInputMapInterface::ID());
}

SubsystemInputMapInterface::Index SubsystemInputMapModel::getIndex(SubsystemInputMapInterface::ID const & id) const
{
    return map.keys().indexOf(id);
}

SubsystemInputMapInterface::Index SubsystemInputMapModel::getIndex(SubsystemInputMapInterface::Item * const item) const
{
    return map.values().indexOf(item);
}

SubsystemInputMapInterface::Item * SubsystemInputMapModel::getItem(SubsystemInputMapInterface::ID const & id) const
{
    return map.value(id, nullptr);
}

SubsystemInputMapInterface::Item * SubsystemInputMapModel::getItem(SubsystemInputMapInterface::Index const & index) const
{
    return ((contains(index)) ? map.values().value(index, nullptr) : nullptr);
}

bool SubsystemInputMapModel::remove(SubsystemInputMapInterface::ID const & id)
{
    if (map.contains(id))
    {
        Q_EMIT removed(map.value(id));

        // trigger signal to remove connection in which input port is involved
        Q_EMIT map.value(id)->removeConnection(map.value(id));

        componentInputs->remove(id);
        delete map.take(id);
        return true;
    }
    return false;
}

bool SubsystemInputMapModel::remove(SubsystemInputMapInterface::Item * const item)
{
    return ((contains(item)) ? remove(getID(item)) : false);
}

bool SubsystemInputMapModel::remove(SubsystemInputMapInterface::Index const & index)
{
    return ((contains(index)) ? remove(getID(index)) : false);
}
