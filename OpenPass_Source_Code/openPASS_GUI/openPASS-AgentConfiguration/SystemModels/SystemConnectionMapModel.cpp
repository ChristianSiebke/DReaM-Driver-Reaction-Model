#include "SystemConnectionMapModel.h"

#include "SystemConnectionItemModel.h"

SystemConnectionMapModel::SystemConnectionMapModel(QObject * const parent)
    : SystemConnectionMapInterface(parent)
{
}

SystemConnectionMapInterface::Iterator SystemConnectionMapModel::begin()
{
    return connections.begin();
}

SystemConnectionMapInterface::ConstIterator SystemConnectionMapModel::begin() const
{
    return connections.begin();
}

SystemConnectionMapInterface::Iterator SystemConnectionMapModel::end()
{
    return connections.end();
}

SystemConnectionMapInterface::ConstIterator SystemConnectionMapModel::end() const
{
    return connections.end();
}

SystemConnectionMapInterface::ID SystemConnectionMapModel::getID(SystemConnectionMapInterface::Item * const item) const
{
    return connections.key(item);
}

SystemConnectionMapInterface::Item * SystemConnectionMapModel::getItem(SystemConnectionMapInterface::ID const & id) const
{
    return connections.value(id, nullptr);
}

bool SystemConnectionMapModel::clear()
{
    qDeleteAll(connections);
    connections.clear();
    Q_EMIT modified();
    return true;
}

bool SystemConnectionMapModel::add(SystemConnectionMapInterface::ID const & id,
                                   SystemConnectionMapInterface::Source const * const source,
                                   SystemConnectionMapInterface::Target const * const target)
{
    if ((!connections.keys().contains(id)) && (!contains(source, target))) //&&
     //       (SystemConnectionItemModel::isCompatible(source, target)))
    {
        connections.insert(id, new SystemConnectionItemModel(source, target, this));

        // Connect the deletion of connections to deletion of components, sources or targets
        connect(target, &SystemConnectionMapInterface::Target::removeConnection,this, &SystemConnectionMapModel::removeTargetConnection);
        connect(source, &SystemConnectionMapInterface::Source::removeConnection,this, &SystemConnectionMapModel::removeSourceConnection);
        connect(source->getComponent(), &SystemComponentItemInterface::removeConnection, this, &SystemConnectionMapModel::removeComponentConnection);
        connect(target->getComponent(), &SystemComponentItemInterface::removeConnection, this, &SystemConnectionMapModel::removeComponentConnection);

        Q_EMIT modified();
        return true;
    }
    return false;
}

bool SystemConnectionMapModel::add(SystemConnectionMapInterface::Source const * const source,
                                   SystemConnectionMapInterface::Target const * const target)
{
    return add(next(), source, target);
}

bool SystemConnectionMapModel::contains(SystemConnectionMapInterface::Source const * const source,
                                        SystemConnectionMapInterface::Target const * const target) const
{
    for (SystemConnectionMapInterface::Item const * const connection : connections)
    {
        if ((connection->getSource() == source) && (connection->getTarget() == target))
        {
            return true;
        }
    }
    return false;
}

void SystemConnectionMapModel::removeTargetConnection(SystemConnectionMapInterface::Target const * const target)
{
    remove(target);
}

void SystemConnectionMapModel::removeSourceConnection(SystemConnectionMapInterface::Source const * const source)
{
    remove(source);
}

void SystemConnectionMapModel::removeComponentConnection(SystemConnectionMapInterface::Component const * const component)
{
    remove(component);
}

bool SystemConnectionMapModel::remove(SystemConnectionMapInterface::Component const * const component)
{
    QList<SystemConnectionMapInterface::ID> ids;
    for (SystemConnectionMapInterface::Item const * const connection : connections)
    {
        if ((connection->getSource()->getComponent() == component) ||
                (connection->getTarget()->getComponent() == component))
        {
            ids << connection->getID();
        }
    }
    return remove(ids);
}

bool SystemConnectionMapModel::remove(SystemConnectionMapInterface::Source const * const source)
{
    QList<SystemConnectionMapInterface::ID> ids;
    for (SystemConnectionMapInterface::Item const * const connection : connections)
    {
        if (connection->getSource() == source)
        {
            ids << connection->getID();
        }
    }
    return remove(ids);
}

bool SystemConnectionMapModel::remove(SystemConnectionMapInterface::Target const * const target)
{
    QList<SystemConnectionMapInterface::ID> ids;
    for (SystemConnectionMapInterface::Item const * const connection : connections)
    {
        if (connection->getTarget() == target)
        {
            ids << connection->getID();
        }
    }
    return remove(ids);
}

SystemConnectionMapInterface::Item * SystemConnectionMapModel::take(SystemConnectionMapInterface::ID const & id)
{
    if(connections.contains(id))
    {
        return connections.take(id);
    }

    return nullptr;
}

SystemConnectionMapInterface::ID SystemConnectionMapModel::next() const
{
    SystemConnectionMapInterface::ID id = 0;
    for (SystemConnectionMapInterface::ID key : connections.keys())
    {
        if (id <= key)
        {
            id = key + 1;
        }
    }
    return id;
}

bool SystemConnectionMapModel::remove(QList<SystemConnectionMapInterface::ID> const & ids)
{
    for (SystemConnectionMapInterface::ID const & id : ids)
    {
        delete connections.take(id);
    }
    if (ids.count() > 0)
    {
        Q_EMIT modified();
    }
    return true;
}


bool SystemConnectionMapModel::remove(SystemConnectionMapInterface::Item * connection)
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

QList<SystemConnectionMapInterface::Item*> SystemConnectionMapModel::values()
{
    return connections.values();
}
