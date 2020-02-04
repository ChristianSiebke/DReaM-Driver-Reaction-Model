/****************************************************************************** 
* Copyright (c) 2017 Volkswagen Group of America. 
* 
* This program and the accompanying materials are made 
* available under the terms of the Eclipse Public License 2.0 
* which is available at https://www.eclipse.org/legal/epl-2.0/ 
* 
* SPDX-License-Identifier: EPL-2.0 
******************************************************************************/ 

#include "SubsystemParameterMapModel.h"

#include "SubsystemParameterItemModel.h"

SubsystemParameterMapModel::SubsystemParameterMapModel(ComponentParameterMapInterface * const componentParameters,
                                                       QObject * const parent)
    : SubsystemParameterMapInterface(parent)
    , componentParameters(componentParameters)
{
}

bool SubsystemParameterMapModel::add(SubsystemParameterMapInterface::ID const & id,
                                     SubsystemParameterMapInterface::SystemComponentParameter * const systemComponentParameterItem)
{
    bool success;

    if (!contains(id))
    {

        // Create a component Parameter Item
        ComponentParameterItemInterface * componentParameterItem = componentParameters->createParameter();

        // Adopt all properties from the selected systemParameterItem
        componentParameterItem->setTitle(systemComponentParameterItem->getTitle());
        componentParameterItem->setUnit(systemComponentParameterItem->getUnit());
        componentParameterItem->setType(systemComponentParameterItem->getType());
        componentParameterItem->setValue(systemComponentParameterItem->getValue());

        // insert the componentParameterItem into the component Map
        success = componentParameters->add(id,componentParameterItem);

        map.insert(id, new SubsystemParameterItemModel(componentParameterItem, systemComponentParameterItem,this));

        Q_EMIT added(map.value(id));

        return true && success;
    }
    return false;
}

bool SubsystemParameterMapModel::add(SubsystemParameterItemInterface::ComponentParameterItem * const componentParameterItem,
                                     SubsystemParameterMapInterface::SystemComponentParameter * const systemComponentParameterItem)
{
    SubsystemParameterMapInterface::ID id = componentParameterItem->getID();
    if(!contains(id))
    {
        map.insert(id, new SubsystemParameterItemModel(componentParameterItem, systemComponentParameterItem,this));

        Q_EMIT added(map.value(id));

        return true;
    }

    return false;
}

bool SubsystemParameterMapModel::add(SubsystemParameterMapInterface::ID const & id,
                                     SubsystemParameterMapInterface::Item * const item)
{
    if (!contains(id))
    {
        componentParameters->add(id, item->getComponentParameter());
        map.insert(id, item);
        item->setParent(this);
        Q_EMIT added(map.value(id));
        return true;
    }
    return false;
}

SubsystemParameterMapInterface::Iterator SubsystemParameterMapModel::begin()
{
    return map.begin();
}

SubsystemParameterMapInterface::ConstIterator SubsystemParameterMapModel::begin() const
{
    return map.begin();
}

bool SubsystemParameterMapModel::clear()
{
    while (map.count() > 0)
    {
        delete map.take(map.keys().first());
    }
    Q_EMIT cleared();
    return true;
}

int SubsystemParameterMapModel::count() const
{
    return map.count();
}

SubsystemParameterMapInterface::Iterator SubsystemParameterMapModel::end()
{
    return map.end();
}

SubsystemParameterMapInterface::ConstIterator SubsystemParameterMapModel::end() const
{
    return map.end();
}

bool SubsystemParameterMapModel::contains(SubsystemParameterMapInterface::ID const & id) const
{
    return map.keys().contains(id);
}

bool SubsystemParameterMapModel::contains(SubsystemParameterMapInterface::Item * const item) const
{
    return map.values().contains(item);
}

bool SubsystemParameterMapModel::contains(SubsystemParameterMapInterface::Index const & index) const
{
    return ((0 <= index) && (index < map.count()));
}

bool SubsystemParameterMapModel::setID(SubsystemParameterMapInterface::Item * const item,
                                       SubsystemParameterMapInterface::ID const & id)
{
    if ((contains(item)) && (!contains(id)))
    {
        map.insert(id, map.take(item->getID()));
        Q_EMIT modifiedID();
        return true;
    }
    return false;
}

SubsystemParameterMapInterface::ID SubsystemParameterMapModel::generateID() const
{
    SubsystemParameterMapInterface::ID id = 0;
    for (SubsystemParameterMapInterface::ID key : map.keys())
    {
        if (id <= key)
        {
            id = key + 1;
        }
    }
    return id;
}

SubsystemParameterMapInterface::ID SubsystemParameterMapModel::getID(SubsystemParameterMapInterface::Item * const item) const
{
    return map.key(item);
}

SubsystemParameterMapInterface::ID SubsystemParameterMapModel::getID(SubsystemParameterMapInterface::Index const & index) const
{
    return ((contains(index)) ? map.keys().value(index) : SubsystemParameterMapInterface::ID());
}

SubsystemParameterMapInterface::Index SubsystemParameterMapModel::getIndex(SubsystemParameterMapInterface::ID const & id) const
{
    return map.keys().indexOf(id);
}

SubsystemParameterMapInterface::Index SubsystemParameterMapModel::getIndex(SubsystemParameterMapInterface::Item * const item) const
{
    return map.values().indexOf(item);
}

SubsystemParameterMapInterface::Item * SubsystemParameterMapModel::getItem(SubsystemParameterMapInterface::ID const & id) const
{
    return map.value(id, nullptr);
}

SubsystemParameterMapInterface::Item * SubsystemParameterMapModel::getItem(SubsystemParameterMapInterface::Index const & index) const
{
    return ((contains(index)) ? map.values().value(index, nullptr) : nullptr);
}

bool SubsystemParameterMapModel::remove(SubsystemParameterMapInterface::ID const & id)
{
    if (map.contains(id))
    {
        Q_EMIT removed();

        componentParameters->remove(id);

        delete map.take(id);
        return true;
    }
    return false;
}

bool SubsystemParameterMapModel::remove(SubsystemParameterMapInterface::Item * const item)
{
    return ((contains(item)) ? remove(getID(item)) : false);
}

bool SubsystemParameterMapModel::remove(SubsystemParameterMapInterface::Index const & index)
{
    return ((contains(index)) ? remove(getID(index)) : false);
}
