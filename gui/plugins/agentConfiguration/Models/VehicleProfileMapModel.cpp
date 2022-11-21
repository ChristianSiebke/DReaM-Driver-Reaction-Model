/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "VehicleProfileMapModel.h"

#include "Models/VehicleProfileItemModel.h"

VehicleProfileMapModel::VehicleProfileMapModel(QObject *parent) :
    VehicleProfileMapInterface(parent)
{
}

VehicleProfileMapInterface::Iterator VehicleProfileMapModel::begin()
{
    return profiles.begin();
}

VehicleProfileMapInterface::ConstIterator VehicleProfileMapModel::constBegin() const
{
    return profiles.constBegin();
}

VehicleProfileMapInterface::Iterator VehicleProfileMapModel::end()
{
    return profiles.end();
}

VehicleProfileMapInterface::ConstIterator VehicleProfileMapModel::constEnd() const
{
    return profiles.constEnd();
}

bool VehicleProfileMapModel::add(const VehicleProfileMapInterface::Name &name)
{
    if (!contains(name))
    {
        profiles.insert(name, new VehicleProfileItemModel(this));
        Q_EMIT added(name);
        return true;
    }

    return false;
}

bool VehicleProfileMapModel::add(const VehicleProfileMapInterface::Name &name, VehicleProfileMapInterface::Item const *const item)
{
    if (!contains(name))
    {
        profiles.insert(name, new VehicleProfileItemModel(item, this));
        Q_EMIT added(name);
        return true;
    }

    return false;
}

bool VehicleProfileMapModel::remove(const VehicleProfileMapInterface::Name &name)
{
    if (contains(name))
    {
        delete profiles.take(name);
        Q_EMIT removed(name);
        return true;
    }

    return false;
}

VehicleProfileMapInterface::Item *VehicleProfileMapModel::getItem(VehicleProfileMapInterface::Name const &name) const
{
    if (contains(name))
    {
        return profiles.value(name);
    }

    return nullptr;
}

VehicleProfileMapInterface::Name VehicleProfileMapModel::getName(Item *const item) const
{
    return profiles.key(item);
}

void VehicleProfileMapModel::setName(Item *const item,
                                     VehicleProfileMapInterface::Name const &name)
{
    Name oldName = getName(item);

    profiles.take(oldName);
    profiles.insert(name, item);

    Q_EMIT modifiedName(oldName, name);
}

bool VehicleProfileMapModel::contains(const VehicleProfileMapInterface::Name &name) const
{
    return profiles.contains(name);
}

QStringList VehicleProfileMapModel::keys() const
{
    return QStringList(profiles.keys());
}

QList<VehicleProfileItemInterface *> VehicleProfileMapModel::values() const
{
    return profiles.values();
}

int VehicleProfileMapModel::count() const
{
    return profiles.count();
}
