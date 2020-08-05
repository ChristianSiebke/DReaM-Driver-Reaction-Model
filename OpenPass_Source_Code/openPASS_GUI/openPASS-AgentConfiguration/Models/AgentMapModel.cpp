/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "AgentMapModel.h"

#include <QStringList>
#include <QMap>

AgentMapModel::AgentMapModel(QObject *parent)
    : AgentMapInterface(parent)
{
}

AgentMapInterface::Iterator AgentMapModel::begin()
{
    return agents.begin();
}

AgentMapInterface::ConstIterator AgentMapModel::constBegin() const
{
    return agents.constBegin();
}

AgentMapInterface::Iterator AgentMapModel::end()
{
    return agents.end();
}

AgentMapInterface::ConstIterator AgentMapModel::constEnd() const
{
    return agents.constEnd();
}


bool AgentMapModel::add(const AgentMapInterface::Name &name)
{
    if(!contains(name))
    {
        agents.insert(name,  new AgentItemModel(this));
        Q_EMIT added(name);
        return true;
    }

    return false;
}


bool AgentMapModel::add(AgentMapInterface::Name const &name, AgentMapInterface::Item const * const item)
{
    if(!contains(name))
    {
        agents.insert(name, new AgentItemModel(item, this));
        Q_EMIT added(name);
        return true;
    }

    return false;
}

bool AgentMapModel::remove(const AgentMapInterface::Name &name)
{
    if(contains(name))
    {
        delete agents.take(name);
        Q_EMIT removed(name);
        return true;
    }

    return false;
}

AgentMapInterface::Item * AgentMapModel::getItem(AgentMapInterface::Name const & name) const
{
    if(contains(name))
    {
        return agents.value(name);
    }

    return nullptr;
}


AgentMapInterface::Name AgentMapModel::getName(Item * const item) const
{
    return agents.key(item);
}

void AgentMapModel::setName(Item * const item,
                            AgentMapInterface::Name const &name)
{

    Name oldName = item->getName();

    agents.take(oldName);
    agents.insert(name, item);

    Q_EMIT modifiedName(oldName, name);
}


bool AgentMapModel::contains(const AgentMapInterface::Name &name) const
{
    return agents.contains(name);
}

QStringList AgentMapModel::keys() const
{
    return QStringList( agents.keys() );
}

QList <AgentItemInterface*> AgentMapModel::values() const
{
    return agents.values();
}

int AgentMapModel::count() const
{
    return agents.count();
}
