/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "DriverProfileMapModel.h"

#include "DriverProfileItemModel.h"

DriverProfileMapModel::DriverProfileMapModel(QObject *parent) :
    DriverProfileMapInterface(parent)
{
}

DriverProfileMapInterface::Iterator DriverProfileMapModel::begin()
{
    return driverProfiles.begin();
}

DriverProfileMapInterface::ConstIterator DriverProfileMapModel::constBegin() const
{
    return driverProfiles.constBegin();
}

DriverProfileMapInterface::Iterator DriverProfileMapModel::end()
{
    return driverProfiles.end();
}

DriverProfileMapInterface::ConstIterator DriverProfileMapModel::constEnd() const
{
    return driverProfiles.constEnd();
}

bool DriverProfileMapModel::add(const DriverProfileMapInterface::Name &name)
{
    if (!contains(name))
    {
        driverProfiles.insert(name, new DriverProfileItemModel(this));
        Q_EMIT added(name);

        return true;
    }

    return false;
}

bool DriverProfileMapModel::add(const DriverProfileMapInterface::Name &name, const DriverProfileMapInterface::Item *const driver)
{
    if (!contains(name))
    {
        driverProfiles.insert(name, new DriverProfileItemModel(driver, this));
        Q_EMIT added(name);
        return true;
    }

    return false;
}

bool DriverProfileMapModel::remove(const DriverProfileMapInterface::Name &name)
{
    if (contains(name))
    {
        delete driverProfiles.take(name);
        Q_EMIT removed(name);
        return true;
    }

    return false;
}

DriverProfileMapInterface::Item *DriverProfileMapModel::getItem(DriverProfileMapInterface::Name const &name) const
{
    if (contains(name))
        return driverProfiles.value(name);

    return nullptr;
}

DriverProfileMapInterface::Name DriverProfileMapModel::getName(Item *const item) const
{
    return driverProfiles.key(item);
}

void DriverProfileMapModel::setName(Item *const item,
                                    DriverProfileMapInterface::Name const &name)
{
    Name oldName = getName(item);

    driverProfiles.take(oldName);
    driverProfiles.insert(name, item);

    Q_EMIT modifiedName(oldName, name);
}

bool DriverProfileMapModel::contains(const DriverProfileMapInterface::Name &name) const
{
    return driverProfiles.contains(name);
}

QStringList DriverProfileMapModel::keys() const
{
    return QStringList(driverProfiles.keys());
}

QList<DriverProfileItemInterface *> DriverProfileMapModel::values() const
{
    return driverProfiles.values();
}

int DriverProfileMapModel::count() const
{
    return driverProfiles.count();
}
