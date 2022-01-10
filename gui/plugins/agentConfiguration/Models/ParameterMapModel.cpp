/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "ParameterMapModel.h"

#include "ParameterItemModel.h"

ParameterMapModel::ParameterMapModel(QObject *parent) :
    ParameterMapInterface(parent)
{
}

ParameterMapInterface::Iterator ParameterMapModel::begin()
{
    return parameters.begin();
}

ParameterMapInterface::ConstIterator ParameterMapModel::constBegin() const
{
    return parameters.constBegin();
}

ParameterMapInterface::Iterator ParameterMapModel::end()
{
    return parameters.end();
}

ParameterMapInterface::ConstIterator ParameterMapModel::constEnd() const
{
    return parameters.constEnd();
}

bool ParameterMapModel::add(const ParameterMapInterface::ID &id)
{
    if (!contains(id))
    {
        parameters.insert(id, new ParameterItemModel(this));
        return true;
    }

    return false;
}

bool ParameterMapModel::add(ParameterMapInterface::Item *const item)
{
    ParameterMapInterface::ID id = generateID();

    if (!contains(id))
    {
        parameters.insert(id, item);
        item->setParent(this);
        return true;
    }

    return false;
}

bool ParameterMapModel::add(const ParameterMapInterface::ID &id, ParameterMapInterface::Item const *const item)
{
    if (!contains(id))
    {
        parameters.insert(id, new ParameterItemModel(item, this));
        return true;
    }

    return false;
}

bool ParameterMapModel::remove(const ParameterMapInterface::ID &id)
{
    if (contains(id))
    {
        delete parameters.take(id);
        return true;
    }

    return false;
}

bool ParameterMapModel::remove(ParameterMapInterface::Item *const item)
{
    if (parameters.values().contains(item))
    {
        ParameterItemInterface::ID id = getID(item);
        delete parameters.take(id);
        return true;
    }

    return false;
}

ParameterMapInterface::Item *ParameterMapModel::getItem(ParameterMapInterface::ID const &id) const
{
    if (contains(id))
        return parameters.value(id);

    return nullptr;
}

ParameterMapInterface::ID ParameterMapModel::getID(Item *const item) const
{
    return parameters.key(item);
}

void ParameterMapModel::setID(Item *const item,
                              ParameterMapInterface::ID const &id)
{
    ParameterMapInterface::ID oldID = getID(item);

    parameters.take(oldID);
    parameters.insert(id, item);
}

ParameterMapInterface::ID ParameterMapModel::generateID()
{
    ParameterMapInterface::ID id = 1;
    for (ParameterMapInterface::ID key : parameters.keys())
    {
        if (id <= key)
        {
            id = key + 1;
        }
    }
    return id;
}

bool ParameterMapModel::contains(const ParameterMapInterface::ID &id) const
{
    return parameters.contains(id);
}

QList<ParameterMapInterface::ID> ParameterMapModel::keys() const
{
    return parameters.keys();
}

QList<ParameterItemInterface *> ParameterMapModel::values() const
{
    return parameters.values();
}

int ParameterMapModel::count() const
{
    return parameters.count();
}

void ParameterMapModel::clear()
{
    qDeleteAll(parameters);
    parameters.clear();
}
