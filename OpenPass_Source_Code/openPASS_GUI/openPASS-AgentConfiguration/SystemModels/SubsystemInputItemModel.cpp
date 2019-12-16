/****************************************************************************** 
* Copyright (c) 2017 Volkswagen Group of America. 
* 
* This program and the accompanying materials are made 
* available under the terms of the Eclipse Public License 2.0 
* which is available at https://www.eclipse.org/legal/epl-2.0/ 
* 
* SPDX-License-Identifier: EPL-2.0 
******************************************************************************/ 

#include "SubsystemInputItemModel.h"

#include "SubsystemInputMapModel.h"

SubsystemInputItemInterface::Type const SubsystemInputItemModel::DefaultType = QStringLiteral("double");
SubsystemInputItemInterface::Title const SubsystemInputItemModel::DefaultTitle = QStringLiteral("Input");
SubsystemInputItemInterface::Unit const SubsystemInputItemModel::DefaultUnit = QStringLiteral("");

SubsystemInputItemModel::SubsystemInputItemModel(ComponentInputItemInterface * const componentInputItem, QObject * const parent)
    : SubsystemInputItemInterface(parent)
    , componentInputItem(componentInputItem)
    , type(DefaultType)
    , title(DefaultTitle)
    , unit(DefaultUnit)
{
    componentInputItem->setType(DefaultType);
    componentInputItem->setTitle(DefaultTitle);
    componentInputItem->setUnit(DefaultUnit);
}

bool SubsystemInputItemModel::clear()
{
    setType(DefaultType);
    setTitle(DefaultTitle);
    setUnit(DefaultUnit);
    componentInputItem->setType(DefaultType);
    componentInputItem->setTitle(DefaultTitle);
    componentInputItem->setUnit(DefaultUnit);
    Q_EMIT cleared();
    return true;
}

bool SubsystemInputItemModel::setID(SubsystemInputItemInterface::ID const & id)
{
    bool success;
    SubsystemInputMapModel * const map =
            qobject_cast<SubsystemInputMapModel * const>(parent());

    success = componentInputItem->setID(id);

    return ((map) ? map->setID(const_cast<SubsystemInputItemModel *>(this), id)
                  : false) && success;
}

SubsystemInputItemInterface::ID SubsystemInputItemModel::getID() const
{
    SubsystemInputMapModel const * const map =
            qobject_cast<SubsystemInputMapModel const * const>(parent());
    return ((map) ? map->getID(const_cast<SubsystemInputItemModel *>(this))
                  : SubsystemInputItemInterface::ID());
}

bool SubsystemInputItemModel::setType(SubsystemInputItemInterface::Type const & _type)
{
    type = _type;
    componentInputItem->setType(_type);
    Q_EMIT modifiedType();
    return true;
}

SubsystemInputItemInterface::Type SubsystemInputItemModel::getType() const
{
    return type;
}

bool SubsystemInputItemModel::setTitle(SubsystemInputItemInterface::Title const & _title)
{
    title = _title;
    componentInputItem->setTitle(_title);
    Q_EMIT modifiedTitle();
    return true;
}

SubsystemInputItemInterface::Title SubsystemInputItemModel::getTitle() const
{
    return title;
}

bool SubsystemInputItemModel::setUnit(SubsystemInputItemInterface::Unit const & _unit)
{
    unit = _unit;
    componentInputItem->setUnit(_unit);
    Q_EMIT modifiedUnit();
    return true;
}

SubsystemInputItemInterface::Unit SubsystemInputItemModel::getUnit() const
{
    return unit;
}

SubsystemInputItemInterface::ComponentInputItem * SubsystemInputItemModel::getComponentInput() const
{
    return componentInputItem;
}
