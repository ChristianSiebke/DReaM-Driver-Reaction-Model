/****************************************************************************** 
* Copyright (c) 2017 Volkswagen Group of America. 
* 
* This program and the accompanying materials are made 
* available under the terms of the Eclipse Public License 2.0 
* which is available at https://www.eclipse.org/legal/epl-2.0/ 
* 
* SPDX-License-Identifier: EPL-2.0 
******************************************************************************/ 

#include "SubsystemOutputConnectionMapModel.h"

#include "SubsystemOutputConnectionItemModel.h"

SubsystemOutputConnectionMapModel::SubsystemOutputConnectionMapModel(QObject * const parent)
    : SubsystemOutputConnectionMapInterface(parent)
{
}

SubsystemOutputConnectionMapInterface::Iterator SubsystemOutputConnectionMapModel::begin()
{
    return connections.begin();
}

SubsystemOutputConnectionMapInterface::ConstIterator SubsystemOutputConnectionMapModel::begin() const
{
    return connections.begin();
}

SubsystemOutputConnectionMapInterface::Iterator SubsystemOutputConnectionMapModel::end()
{
    return connections.end();
}

SubsystemOutputConnectionMapInterface::ConstIterator SubsystemOutputConnectionMapModel::end() const
{
    return connections.end();
}

SubsystemOutputConnectionMapInterface::ID SubsystemOutputConnectionMapModel::getID(SubsystemOutputConnectionMapInterface::Item * const item) const
{
    return connections.key(item);
}

SubsystemOutputConnectionMapInterface::Item * SubsystemOutputConnectionMapModel::getItem(SubsystemOutputConnectionMapInterface::ID const & id) const
{
    return connections.value(id, nullptr);
}

bool SubsystemOutputConnectionMapModel::clear()
{
    qDeleteAll(connections);
    connections.clear();
    Q_EMIT modified();
    return true;
}

bool SubsystemOutputConnectionMapModel::add(SubsystemOutputConnectionMapInterface::ID const & id,
                                   SubsystemOutputConnectionMapInterface::Source const * const source,
                                   SubsystemOutputConnectionMapInterface::Target const * const target)
{
    if ((!connections.keys().contains(id)) && (!contains(source, target)))
    {
        connections.insert(id, new SubsystemOutputConnectionItemModel(source, target, this));

        // connect the deletion of connections to deletion of sources, targets and components
        connect(source, &SubsystemOutputConnectionMapInterface::Source::removeConnection, this, &SubsystemOutputConnectionMapModel::removeSourceConnection);
        connect(target, &SubsystemOutputConnectionMapInterface::Target::removeConnection, this, &SubsystemOutputConnectionMapModel::removeTargetConnection);
        connect(source->getComponent(), &SystemComponentItemInterface::removeConnection, this, &SubsystemOutputConnectionMapModel::removeComponentConnection);

        Q_EMIT modified();
        return true;
    }
    return false;
}

bool SubsystemOutputConnectionMapModel::add(SubsystemOutputConnectionMapInterface::Source const * const source,
                                   SubsystemOutputConnectionMapInterface::Target const * const target)
{
    return add(next(), source, target);
}

bool SubsystemOutputConnectionMapModel::contains(SubsystemOutputConnectionMapInterface::Source const * const source,
                                        SubsystemOutputConnectionMapInterface::Target const * const target) const
{
    for (SubsystemOutputConnectionMapInterface::Item const * const connection : connections)
    {
        if ((connection->getSource() == source) && (connection->getTarget() == target))
        {
            return true;
        }
    }
    return false;
}

void SubsystemOutputConnectionMapModel::removeSourceConnection(SubsystemOutputConnectionMapInterface::Source const * const source)
{
    remove(source);
}

void SubsystemOutputConnectionMapModel::removeTargetConnection(SubsystemOutputConnectionMapInterface::Target const * const target)
{
    remove(target);
}

void SubsystemOutputConnectionMapModel::removeComponentConnection(SubsystemOutputConnectionMapInterface::Component const * const component)
{
    remove(component);
}

bool SubsystemOutputConnectionMapModel::remove(SubsystemOutputConnectionMapInterface::Component const * const component)
{
    QList<SubsystemOutputConnectionMapInterface::ID> ids;
    for (SubsystemOutputConnectionMapInterface::Item const * const connection : connections)
    {
        if ((connection->getSource()->getComponent() == component))
        {
            ids << connection->getID();
        }
    }
    return remove(ids);
}

bool SubsystemOutputConnectionMapModel::remove(SubsystemOutputConnectionMapInterface::Source const * const source)
{
    QList<SubsystemOutputConnectionMapInterface::ID> ids;
    for (SubsystemOutputConnectionMapInterface::Item const * const connection : connections)
    {
        if (connection->getSource() == source)
        {
            ids << connection->getID();
        }
    }
    return remove(ids);
}

bool SubsystemOutputConnectionMapModel::remove(SubsystemOutputConnectionMapInterface::Target const * const target)
{
    QList<SubsystemOutputConnectionMapInterface::ID> ids;
    for (SubsystemOutputConnectionMapInterface::Item const * const connection : connections)
    {
        if (connection->getTarget() == target)
        {
            ids << connection->getID();
        }
    }
    return remove(ids);
}

SubsystemOutputConnectionMapInterface::ID SubsystemOutputConnectionMapModel::next() const
{
    SubsystemOutputConnectionMapInterface::ID id = 0;
    for (SubsystemOutputConnectionMapInterface::ID key : connections.keys())
    {
        if (id <= key)
        {
            id = key + 1;
        }
    }
    return id;
}

bool SubsystemOutputConnectionMapModel::remove(QList<SubsystemOutputConnectionMapInterface::ID> const & ids)
{
    for (SubsystemOutputConnectionMapInterface::ID const & id : ids)
    {
        delete connections.take(id);
    }
    if (ids.count() > 0)
    {
        Q_EMIT modified();
    }
    return true;
}

bool SubsystemOutputConnectionMapModel::remove(SubsystemOutputConnectionMapInterface::Item * connection)
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

QList<SubsystemOutputConnectionMapInterface::Item*> SubsystemOutputConnectionMapModel::values()
{
    return connections.values();
}
