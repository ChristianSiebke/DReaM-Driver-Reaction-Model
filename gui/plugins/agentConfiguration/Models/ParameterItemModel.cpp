/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "ParameterItemModel.h"

#include "openPASS-AgentConfiguration/ParameterMapInterface.h"

ParameterItemInterface::Type ParameterItemModel::default_type = QString("Double");
ParameterItemInterface::Key ParameterItemModel::default_key = QString("parameter");
ParameterItemInterface::Value ParameterItemModel::default_value = QString("0.0");
ParameterItemInterface::Value ParameterItemModel::default_unit = QString("");

ParameterItemModel::ParameterItemModel(QObject *parent) :
    ParameterItemInterface(parent), type(default_type), key(default_key), value(default_value), unit(default_unit)
{
}

ParameterItemModel::ParameterItemModel(ParameterItemInterface const *const parameter, QObject *parent) :
    ParameterItemInterface(parent), type(parameter->getType()), key(parameter->getKey()), value(parameter->getValue()), unit(parameter->getUnit())
{
}

ParameterItemInterface::ID ParameterItemModel::getID() const
{
    ParameterMapInterface const *const map = qobject_cast<ParameterMapInterface const *const>(parent());

    return ((map) ? map->getID(const_cast<ParameterItemModel *>(this)) : ParameterItemInterface::ID());
}

ParameterItemInterface::Type ParameterItemModel::getType() const
{
    return type;
}

void ParameterItemModel::setType(ParameterItemInterface::Type const &_type)
{
    type = _type;
}

ParameterItemInterface::Key ParameterItemModel::getKey() const
{
    return key;
}

void ParameterItemModel::setKey(ParameterItemInterface::Key const &_key)
{
    key = _key;
}

ParameterItemInterface::Value ParameterItemModel::getValue() const
{
    return value;
}

void ParameterItemModel::setValue(ParameterItemInterface::Value const &_value)
{
    value = _value;
}

ParameterItemInterface::Unit ParameterItemModel::getUnit() const
{
    return unit;
}

void ParameterItemModel::setUnit(ParameterItemInterface::Value const &_unit)
{
    unit = _unit;
}
