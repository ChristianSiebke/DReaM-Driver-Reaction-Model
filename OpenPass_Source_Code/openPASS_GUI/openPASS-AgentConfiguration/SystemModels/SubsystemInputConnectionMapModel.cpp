/****************************************************************************** 
* Copyright (c) 2017 Volkswagen Group of America. 
* 
* This program and the accompanying materials are made 
* available under the terms of the Eclipse Public License 2.0 
* which is available at https://www.eclipse.org/legal/epl-2.0/ 
* 
* SPDX-License-Identifier: EPL-2.0 
******************************************************************************/ 

#include "SubsystemInputConnectionMapModel.h"

#include "SubsystemInputConnectionItemModel.h"

SubsystemInputConnectionMapModel::SubsystemInputConnectionMapModel(QObject * const parent)
    : SubsystemInputConnectionMapInterface(parent)
{
}

SubsystemInputConnectionMapInterface::Iterator SubsystemInputConnectionMapModel::begin()
{
    return connections.begin();
}

SubsystemInputConnectionMapInterface::ConstIterator SubsystemInputConnectionMapModel::begin() const
{
    return connections.begin();
}

SubsystemInputConnectionMapInterface::Iterator SubsystemInputConnectionMapModel::end()
{
    return connections.end();
}

SubsystemInputConnectionMapInterface::ConstIterator SubsystemInputConnectionMapModel::end() const
{
    return connections.end();
}

SubsystemInputConnectionMapInterface::ID SubsystemInputConnectionMapModel::getID(SubsystemInputConnectionMapInterface::Item * const item) const
{
    return connections.key(item);
}

SubsystemInputConnectionMapInterface::Item * SubsystemInputConnectionMapModel::getItem(SubsystemInputConnectionMapInterface::ID const & id) const
{
    return connections.value(id, nullptr);
}

bool SubsystemInputConnectionMapModel::clear()
{
    qDeleteAll(connections);
    connections.clear();
    Q_EMIT modified();
    return true;
}

bool SubsystemInputConnectionMapModel::add(SubsystemInputConnectionMapInterface::ID const & id,
                                   SubsystemInputConnectionMapInterface::Source const * const source,
                                   SubsystemInputConnectionMapInterface::Target const * const target)
{
    if ((!connections.keys().contains(id)) && (!contains(source, target)) )//&&
          //  (SubsystemInputConnectionItemModel::isCompatible(source, target)))
    {
        connections.insert(id, new SubsystemInputConnectionItemModel(source, target, this));

        // connect the deletion of connections to deletion of sources, targets and components
        connect(source, &SubsystemInputConnectionMapInterface::Source::removeConnection, this, &SubsystemInputConnectionMapModel::removeSourceConnection);
        connect(target, &SubsystemInputConnectionMapInterface::Target::removeConnection, this, &SubsystemInputConnectionMapModel::removeTargetConnection);
        connect(target->getComponent(), &SystemComponentItemInterface::removeConnection, this, &SubsystemInputConnectionMapModel::removeComponentConnection);

        Q_EMIT modified();
        return true;
    }
    return false;
}

bool SubsystemInputConnectionMapModel::add(SubsystemInputConnectionMapInterface::Source const * const source,
                                   SubsystemInputConnectionMapInterface::Target const * const target)
{
    return add(next(), source, target);
}

bool SubsystemInputConnectionMapModel::contains(SubsystemInputConnectionMapInterface::Source const * const source,
                                        SubsystemInputConnectionMapInterface::Target const * const target) const
{
    for (SubsystemInputConnectionMapInterface::Item const * const connection : connections)
    {
        if ((connection->getSource() == source) && (connection->getTarget() == target))
        {
            return true;
        }
    }
    return false;
}

void SubsystemInputConnectionMapModel::removeSourceConnection(SubsystemInputConnectionMapInterface::Source const * const source)
{
    remove(source);
}

void SubsystemInputConnectionMapModel::removeTargetConnection(SubsystemInputConnectionMapInterface::Target const * const target)
{
    remove(target);
}

void SubsystemInputConnectionMapModel::removeComponentConnection(SubsystemInputConnectionMapInterface::Component const * const component)
{
    remove(component);
}


bool SubsystemInputConnectionMapModel::remove(SubsystemInputConnectionMapInterface::Component const * const component)
{
    QList<SubsystemInputConnectionMapInterface::ID> ids;
    for (SubsystemInputConnectionMapInterface::Item const * const connection : connections)
    {
        if ((connection->getTarget()->getComponent() == component))
        {
            ids << connection->getID();
        }
    }
    return remove(ids);
}

bool SubsystemInputConnectionMapModel::remove(SubsystemInputConnectionMapInterface::Source const * const source)
{
    QList<SubsystemInputConnectionMapInterface::ID> ids;
    for (SubsystemInputConnectionMapInterface::Item const * const connection : connections)
    {
        if (connection->getSource() == source)
        {
            ids << connection->getID();
        }
    }
    return remove(ids);
}

bool SubsystemInputConnectionMapModel::remove(SubsystemInputConnectionMapInterface::Target const * const target)
{
    QList<SubsystemInputConnectionMapInterface::ID> ids;
    for (SubsystemInputConnectionMapInterface::Item const * const connection : connections)
    {
        if (connection->getTarget() == target)
        {
            ids << connection->getID();
        }
    }
    return remove(ids);
}

SubsystemInputConnectionMapInterface::ID SubsystemInputConnectionMapModel::next() const
{
    SubsystemInputConnectionMapInterface::ID id = 0;
    for (SubsystemInputConnectionMapInterface::ID key : connections.keys())
    {
        if (id <= key)
        {
            id = key + 1;
        }
    }
    return id;
}

bool SubsystemInputConnectionMapModel::remove(QList<SubsystemInputConnectionMapInterface::ID> const & ids)
{
    for (SubsystemInputConnectionMapInterface::ID const & id : ids)
    {
        delete connections.take(id);
    }
    if (ids.count() > 0)
    {
        Q_EMIT modified();
    }
    return true;
}

bool SubsystemInputConnectionMapModel::remove(SubsystemInputConnectionMapInterface::Item * connection)
{
    if( connections.values().contains(connection) )
    {
        delete connections.take( connection->getID());
        Q_EMIT modified();
        return true;
    }
    else
        return false;
}

QList<SubsystemInputConnectionMapInterface::Item*> SubsystemInputConnectionMapModel::values()
{
    return connections.values();
}
