/********************************************************************************
 * Copyright (c) 2017 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "Models/SystemMapModel.h"

#include "Models/SystemItemModel.h"

SystemMapModel::SystemMapModel(QObject *const parent) :
    SystemMapInterface(parent)
{
}

SystemMapInterface::Iterator SystemMapModel::begin()
{
    return systems.begin();
}

SystemMapInterface::ConstIterator SystemMapModel::begin() const
{
    return systems.begin();
}

bool SystemMapModel::clear()
{
    Q_EMIT cleared();

    // delete all systems
    qDeleteAll(systems);

    // clear the map
    systems.clear();

    return true;
}

SystemMapInterface::Iterator SystemMapModel::end()
{
    return systems.end();
}

SystemMapInterface::ConstIterator SystemMapModel::end() const
{
    return systems.end();
}

SystemMapInterface::ID SystemMapModel::getID(SystemMapInterface::Item *const item) const
{
    return systems.key(item);
}

SystemMapInterface::Item *SystemMapModel::getItem(SystemMapInterface::ID const &id) const
{
    return systems.value(id, nullptr);
}

bool SystemMapModel::add(SystemMapModel::ID const &id)
{
    if (!contains(id))
    {
        SystemMapModel::Item *system = new SystemItemModel(this);
        systems.insert(id, system);
        Q_EMIT added(system);
        return true;
    }
    return false;
}

bool SystemMapModel::add(SystemMapInterface::ID const &id,
                         SystemMapInterface::Item *const system)
{
    if (!contains(id))
    {
        system->setParent(this);
        systems.insert(id, system);
        Q_EMIT added(system);
        return true;
    }
    return false;
}

bool SystemMapModel::remove(SystemMapInterface::ID const &id)
{
    if (systems.keys().contains(id))
    {
        SystemMapInterface::Item *const system = systems.take(id);
        delete system;
        return true;
    }
    return false;
}

bool SystemMapModel::contains(SystemItemInterface::Title const &title)
{
    int k;
    for (k = 0; k < count(); k++)
    {
        if (title == getItem(k)->getTitle())
            return true;
    }

    return false;
}

bool SystemMapModel::contains(SystemItemInterface::ID const &id)
{
    return systems.contains(id);
}

SystemMapInterface::ID SystemMapModel::generateID()
{
    SystemMapInterface::ID id = 0;
    for (SystemMapInterface::ID key : systems.keys())
    {
        if (id <= key)
        {
            id = key + 1;
        }
    }
    return id;
}

SystemItemInterface::Title SystemMapModel::generateTitle()
{
    int name_id = 1;
    SystemItemInterface::Title name = QString("System %1").arg(name_id);
    QList<SystemItemInterface::Title> NameList;

    for (Item *item : systems.values())
    {
        NameList << item->getTitle();
    }

    // Determine free name with pattern System X with X being the lowest unused number
    while (NameList.contains(name))
    {
        name_id++;
        name = QString("System %1").arg(name_id);
    }

    return name;
}

int SystemMapModel::count() const
{
    return systems.count();
}

QList<SystemMapInterface::Item *> SystemMapModel::values() const
{
    return systems.values();
}
