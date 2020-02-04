/****************************************************************************** 
* Copyright (c) 2017 Volkswagen Group of America. 
* 
* This program and the accompanying materials are made 
* available under the terms of the Eclipse Public License 2.0 
* which is available at https://www.eclipse.org/legal/epl-2.0/ 
* 
* SPDX-License-Identifier: EPL-2.0 
******************************************************************************/ 

#include "SubsystemParameterItemModel.h"

#include "SubsystemParameterMapModel.h"

#include <iostream>


SubsystemParameterItemModel::SubsystemParameterItemModel(ComponentParameterItemInterface * const componentParameterItem,
                                                         SystemComponentParameterItemInterface * const systemComponentParameterItem,
                                                         QObject * const parent)
    : SubsystemParameterItemInterface(parent)
    , componentParameterItem(componentParameterItem)
    , systemComponentParameterItem(systemComponentParameterItem)
    , type(systemComponentParameterItem->getType())
    , title(systemComponentParameterItem->getTitle())
    , unit(systemComponentParameterItem->getUnit())
    , value(systemComponentParameterItem->getValue())
{
}

bool SubsystemParameterItemModel::setID(SubsystemParameterItemInterface::ID const & id)
{
    SubsystemParameterMapModel * const map =
            qobject_cast<SubsystemParameterMapModel * const>(parent());
    return ((map) ? map->setID(const_cast<SubsystemParameterItemModel *>(this), id)
                  : false);
}

SubsystemParameterItemInterface::ID SubsystemParameterItemModel::getID() const
{
    SubsystemParameterMapModel const * const map =
            qobject_cast<SubsystemParameterMapModel const * const>(parent());
    return ((map) ? map->getID(const_cast<SubsystemParameterItemModel *>(this))
                  : SubsystemParameterItemInterface::ID());
}

SubsystemParameterItemInterface::Type SubsystemParameterItemModel::getType() const
{
    return type;
}

bool SubsystemParameterItemModel::setTitle(SubsystemParameterItemInterface::Title const & _title)
{
    title = _title;
    componentParameterItem->setTitle(_title);
    Q_EMIT modifiedTitle();
    return true;
}

SubsystemParameterItemInterface::Title SubsystemParameterItemModel::getTitle() const
{
    return title;
}


SubsystemParameterItemInterface::Unit SubsystemParameterItemModel::getUnit() const
{
    return unit;
}

bool SubsystemParameterItemModel::setValue(SubsystemParameterItemInterface::Value const & _value)
{
    value = _value;
    componentParameterItem->setValue(_value);
    systemComponentParameterItem->setValue(_value);

    Q_EMIT modifiedValue();
    return true;
}

SubsystemParameterItemInterface::Value SubsystemParameterItemModel::getValue() const
{
    return value;
}

SubsystemParameterItemInterface::ComponentParameterItem * SubsystemParameterItemModel::getComponentParameter() const
{
    return componentParameterItem;
}

SubsystemParameterItemInterface::SystemComponentParameterItem * SubsystemParameterItemModel::getSystemComponentParameter() const
{
    return systemComponentParameterItem;
}


