/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef VEHICLECOMPONENTPROFILEMAPINTERFACE_H
#define VEHICLECOMPONENTPROFILEMAPINTERFACE_H

#include "openPASS-AgentConfiguration/VehicleComponentProfileItemInterface.h"

#include <QObject>
#include <QPair>
#include <QMap>

class VehicleComponentProfileMapInterface: public QObject
{

    Q_OBJECT

public:
    using Name = VehicleComponentProfileItemInterface::Name;
    using Type = VehicleComponentProfileItemInterface::Type;
    using ID = QPair<Name, Type>;
    using Item = VehicleComponentProfileItemInterface;

public:
    using Map = QMap<ID, Item *>;
    using Iterator = Map::Iterator;
    using ConstIterator = Map::ConstIterator;

public:
    VehicleComponentProfileMapInterface(QObject * const parent = nullptr) : QObject(parent){}
    virtual ~VehicleComponentProfileMapInterface() = default;


public:
    virtual VehicleComponentProfileMapInterface::Iterator begin() = 0;
    virtual VehicleComponentProfileMapInterface::ConstIterator constBegin() const = 0;

public:
    virtual VehicleComponentProfileMapInterface::Iterator end() = 0;
    virtual VehicleComponentProfileMapInterface::ConstIterator constEnd() const = 0;

public:
    virtual bool add(VehicleComponentProfileMapInterface::ID const & id) = 0;

public:
    virtual bool remove(VehicleComponentProfileMapInterface::ID const & id) = 0;
    virtual bool remove(VehicleComponentProfileMapInterface::Item * const item) = 0;

public:
    virtual VehicleComponentProfileMapInterface::Item * getItem(VehicleComponentProfileMapInterface::ID const & id) const = 0;

public:
    virtual void setID(VehicleComponentProfileMapInterface::Item  * const item,
                         VehicleComponentProfileMapInterface::ID const & id) = 0;

    virtual VehicleComponentProfileMapInterface::ID getID(VehicleComponentProfileMapInterface::Item  * const item) const = 0;

public:
    virtual bool contains(VehicleComponentProfileMapInterface::ID const & id) const = 0;
    virtual int count() const = 0;

public:
    virtual void clear() = 0;

public:
    virtual QList<ID> keys() const =0;
    virtual QList<Item*> values() const =0;
};

#endif // VEHICLECOMPONENTPROFILEMAPINTERFACE_H
