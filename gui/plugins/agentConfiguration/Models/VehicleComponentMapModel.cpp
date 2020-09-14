/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "VehicleComponentMapModel.h"

#include "Models/VehicleComponentItemModel.h"

VehicleComponentMapModel::VehicleComponentMapModel(QObject *parent) :
    VehicleComponentMapInterface(parent)
{
}

VehicleComponentMapInterface::Iterator VehicleComponentMapModel::begin()
{
    return components.begin();
}

VehicleComponentMapInterface::ConstIterator VehicleComponentMapModel::constBegin() const
{
    return components.constBegin();
}

VehicleComponentMapInterface::Iterator VehicleComponentMapModel::end()
{
    return components.end();
}

VehicleComponentMapInterface::ConstIterator VehicleComponentMapModel::constEnd() const
{
    return components.constEnd();
}

bool VehicleComponentMapModel::add(const VehicleComponentMapInterface::Type &type)
{
    if (!contains(type))
    {
        components.insert(type, new VehicleComponentItemModel(this));
        return true;
    }

    return false;
}

bool VehicleComponentMapModel::remove(const VehicleComponentMapInterface::Type &type)
{
    if (contains(type))
    {
        delete components.take(type);
        return true;
    }

    return false;
}

VehicleComponentMapInterface::Item *VehicleComponentMapModel::getItem(VehicleComponentMapInterface::Type const &type) const
{
    if (contains(type))
        return components.value(type);

    return nullptr;
}

VehicleComponentMapInterface::Type VehicleComponentMapModel::getType(Item *const item) const
{
    return components.key(item);
}

void VehicleComponentMapModel::setType(Item *const item,
                                       VehicleComponentMapInterface::Type const &type)
{
    Type oldType = item->getType();

    components.take(oldType);
    components.insert(type, item);
}

bool VehicleComponentMapModel::contains(const VehicleComponentMapInterface::Type &type) const
{
    return components.contains(type);
}

QStringList VehicleComponentMapModel::keys() const
{
    return QStringList(components.keys());
}

QList<VehicleComponentItemInterface *> VehicleComponentMapModel::values() const
{
    return components.values();
}

int VehicleComponentMapModel::count() const
{
    return components.count();
}

void VehicleComponentMapModel::clear()
{
    qDeleteAll(components);
    components.clear();
}
