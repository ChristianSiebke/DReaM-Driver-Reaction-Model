/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef VEHICLECOMPONENTMAPINTERFACE_H
#define VEHICLECOMPONENTMAPINTERFACE_H

#include "openPASS-AgentConfiguration/VehicleComponentItemInterface.h"

#include <QObject>
#include <QMap>

class VehicleComponentMapInterface: public QObject
{
    Q_OBJECT

public:
    using Type = VehicleComponentItemInterface::Type;
    using Item = VehicleComponentItemInterface;

public:
    using Map = QMap<Type, Item *>;
    using Iterator = Map::Iterator;
    using ConstIterator = Map::ConstIterator;

public:
    VehicleComponentMapInterface(QObject * const parent = nullptr) : QObject(parent){}
    virtual ~VehicleComponentMapInterface() = default;


public:
    virtual VehicleComponentMapInterface::Iterator begin() = 0;
    virtual VehicleComponentMapInterface::ConstIterator constBegin() const = 0;

public:
    virtual VehicleComponentMapInterface::Iterator end() = 0;
    virtual VehicleComponentMapInterface::ConstIterator constEnd() const = 0;

public:
    virtual bool add(VehicleComponentMapInterface::Type const & type) = 0;
 //   virtual bool add(VehicleComponentMapInterface::Item * const item) = 0;

public:
    virtual bool remove(VehicleComponentMapInterface::Type const & type) = 0;
    virtual bool remove(VehicleComponentMapInterface::Item * const item) = 0;

public:
    virtual VehicleComponentMapInterface::Item * getItem(VehicleComponentMapInterface::Type const & type) const = 0;

public:
    virtual void setType(VehicleComponentMapInterface::Item  * const item,
                         VehicleComponentMapInterface::Type const & type) = 0;

    virtual VehicleComponentMapInterface::Type getType(VehicleComponentMapInterface::Item  * const item) const = 0;

public:
    virtual bool contains (VehicleComponentMapInterface::Type const & type) const =0;
    virtual int count() const =0;

public:
    virtual void clear() =0;

public:
    virtual QStringList keys() const =0;
    virtual QList<Item*> values() const =0;
};

#endif // VEHICLECOMPONENTMAPINTERFACE_H


