/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "SensorProfileMapModel.h"

#include "SensorProfileItemModel.h"

SensorProfileMapModel::SensorProfileMapModel(QObject *parent) :
    SensorProfileMapInterface(parent)
{
}

SensorProfileMapInterface::Iterator SensorProfileMapModel::begin()
{
    return sensorProfiles.begin();
}

SensorProfileMapInterface::ConstIterator SensorProfileMapModel::constBegin() const
{
    return sensorProfiles.constBegin();
}

SensorProfileMapInterface::Iterator SensorProfileMapModel::end()
{
    return sensorProfiles.end();
}

SensorProfileMapInterface::ConstIterator SensorProfileMapModel::constEnd() const
{
    return sensorProfiles.constEnd();
}

bool SensorProfileMapModel::add(const SensorProfileMapInterface::ID &id)
{
    if (!contains(id))
    {
        sensorProfiles.insert(id, new SensorProfileItemModel(this));
        return true;
    }

    return false;
}

bool SensorProfileMapModel::remove(const SensorProfileMapInterface::ID &id)
{
    if (contains(id))
    {
        delete sensorProfiles.take(id);
        return true;
    }

    return false;
}

SensorProfileMapInterface::Item *SensorProfileMapModel::getItem(SensorProfileMapInterface::ID const &id) const
{
    if (contains(id))
        return sensorProfiles.value(id);

    return nullptr;
}

SensorProfileMapInterface::ID SensorProfileMapModel::getID(Item *const item) const
{
    return sensorProfiles.key(item);
}

void SensorProfileMapModel::setID(Item *const item,
                                  SensorProfileMapInterface::ID const &id)
{
    SensorProfileMapInterface::ID oldID = getID(item);

    sensorProfiles.take(oldID);
    sensorProfiles.insert(id, item);
}

SensorProfileMapInterface::Name SensorProfileMapModel::generateName(SensorProfileMapInterface::Type type)
{
    int index = 1;
    QString name = QString("Standard %1").arg(index);
    SensorProfileMapInterface::ID id;

    id = SensorProfileMapInterface::ID(name, type);

    while (contains(id))
    {
        index++;
        name = QString("Standard %1").arg(index);
        id = SensorProfileMapInterface::ID(name, type);
    }

    return name;
}

bool SensorProfileMapModel::contains(const SensorProfileMapInterface::ID &id) const
{
    return sensorProfiles.contains(id);
}

QList<SensorProfileMapInterface::ID> SensorProfileMapModel::keys() const
{
    return sensorProfiles.keys();
}

QList<SensorProfileItemInterface *> SensorProfileMapModel::values() const
{
    return sensorProfiles.values();
}

int SensorProfileMapModel::count() const
{
    return sensorProfiles.count();
}

void SensorProfileMapModel::clear()
{
    qDeleteAll(sensorProfiles);
    sensorProfiles.clear();
}
