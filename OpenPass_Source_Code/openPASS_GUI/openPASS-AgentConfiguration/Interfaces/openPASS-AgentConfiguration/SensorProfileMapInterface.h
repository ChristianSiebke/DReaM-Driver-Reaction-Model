/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef SENSORPROFILEMAPINTERFACE_H
#define SENSORPROFILEMAPINTERFACE_H

#include "openPASS-AgentConfiguration/SensorProfileItemInterface.h"

#include <QObject>
#include <QPair>
#include <QMap>

class SensorProfileMapInterface: public QObject
{

Q_OBJECT

public:
    using Name = SensorProfileItemInterface::Name;
    using Type = SensorProfileItemInterface::Type;
    using ID = QPair<Name, Type>;
    using Item = SensorProfileItemInterface;

public:
    using Map = QMap<ID, Item *>;
    using Iterator = Map::Iterator;
    using ConstIterator = Map::ConstIterator;

public:
    SensorProfileMapInterface(QObject * const parent = nullptr) : QObject(parent){}
    virtual ~SensorProfileMapInterface() = default;


public:
    virtual SensorProfileMapInterface::Iterator begin() = 0;
    virtual SensorProfileMapInterface::ConstIterator constBegin() const = 0;

public:
    virtual SensorProfileMapInterface::Iterator end() = 0;
    virtual SensorProfileMapInterface::ConstIterator constEnd() const = 0;

public:
    virtual bool add(SensorProfileMapInterface::ID const & id) = 0;
//    virtual bool add(SensorProfileMapInterface::Item * const item) = 0;

public:
    virtual bool remove(SensorProfileMapInterface::ID const & id) = 0;
    virtual bool remove(SensorProfileMapInterface::Item * const item) = 0;

public:
    virtual SensorProfileMapInterface::Item * getItem(SensorProfileMapInterface::ID const & id) const = 0;

public:
    virtual void setID(SensorProfileMapInterface::Item  * const item,
                         SensorProfileMapInterface::ID const & id) = 0;

    virtual SensorProfileMapInterface::ID getID(SensorProfileMapInterface::Item  * const item) const = 0;

    virtual Name generateName(Type type) =0;

public:
    virtual bool contains (SensorProfileMapInterface::ID const & id) const =0;
    virtual int count() const =0;

public:
    virtual void clear() =0;

public:
    virtual QList<ID> keys() const =0;
    virtual QList<Item*> values() const =0;

};


#endif // SENSORPROFILEMAPINTERFACE_H
