#include "SystemComponentMapModel.h"

#include "SystemComponentItemModel.h"
#include "SystemConnectionMapModel.h"

#include <iostream>

SystemComponentMapModel::SystemComponentMapModel(SystemConnectionMapInterface * const connections,
                                                 QObject * const parent)
    : SystemComponentMapInterface(parent)
    , connections(connections)
{
}

bool SystemComponentMapModel::add(SystemComponentMapInterface::ID const & id,
                                  SystemComponentMapInterface::Item * const component)
{
    if (!components.keys().contains(id))
    {
        component->setParent(this);
        Q_EMIT added(*components.insert(id, component));
        return true;
    }
    return false;
}

bool SystemComponentMapModel::add(ComponentItemInterface const * const component,
                                  SystemComponentItemInterface::Position const & position)
{
    Q_EMIT added(*components.insert(next(), new SystemComponentItemModel(
                                        component, position, this)));
    return true;
}

SystemComponentMapInterface::Iterator SystemComponentMapModel::begin()
{
    return components.begin();
}

SystemComponentMapInterface::ConstIterator SystemComponentMapModel::begin() const
{
    return components.begin();
}

bool SystemComponentMapModel::clear()
{
    if (connections->clear())
    {
        qDeleteAll(components);
        components.clear();
        Q_EMIT cleared();
        return true;
    }
    return false;
}

bool SystemComponentMapModel::contains(SystemComponentMapInterface::ID const &id) const
{
    return components.keys().contains(id);
}

SystemComponentMapInterface::Iterator SystemComponentMapModel::end()
{
    return components.end();
}

SystemComponentMapInterface::ConstIterator SystemComponentMapModel::end() const
{
    return components.end();
}

SystemComponentMapInterface::ID SystemComponentMapModel::getID(SystemComponentMapInterface::Item * const item) const
{
    return components.key(item);
}

SystemComponentMapInterface::Item * SystemComponentMapModel::getItem(SystemComponentMapInterface::ID const & id) const
{
    return components.value(id, nullptr);
}

bool SystemComponentMapModel::remove(SystemComponentMapInterface::ID const & id)
{
    if (components.contains(id))
    {
        // send signal to eliminate the view
        Q_EMIT removed(components.value(id));

        // remove connections in which component is involved
        Q_EMIT components.value(id)->removeConnection(components.value(id));
      //  connections->remove(components.value(id));

        // remove component from map and delete the item
        delete components.take(id);

        return true;
    }
    return false;
}

SystemComponentMapInterface::Item * SystemComponentMapModel::take(const SystemComponentMapInterface::ID &id)
{
    if(components.contains(id))
    {
        // send Signal to eliminate view
        Q_EMIT removed(components.value(id));

        return components.take(id);
    }

    return nullptr;
}


QList <SystemComponentMapInterface::Item *> SystemComponentMapModel::values()
{
    return components.values();
}

bool SystemComponentMapModel::duplicate(SystemComponentItemModel const * const component,
                                        SystemComponentItemInterface::Position const & position)
{
    Q_EMIT added(*components.insert(next(), new SystemComponentItemModel(
                                            *component, position, this)));
    return true;
}

SystemComponentMapInterface::ID SystemComponentMapModel::next() const
{
    SystemComponentMapInterface::ID id = 0;
    for (SystemComponentMapInterface::ID key : components.keys())
    {
        if (id <= key)
        {
            id = key + 1;
        }
    }
    return id;
}

SystemComponentMapInterface::ID SystemComponentMapModel::lastID()
{
    return components.lastKey();
}
