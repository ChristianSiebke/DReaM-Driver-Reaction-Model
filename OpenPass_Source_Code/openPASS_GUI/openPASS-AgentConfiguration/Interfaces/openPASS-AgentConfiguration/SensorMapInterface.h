/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef SENSORMAPINTERFACE_H
#define SENSORMAPINTERFACE_H

#include "openPASS-AgentConfiguration/SensorItemInterface.h"

#include <QObject>
#include <QMap>

class SensorMapInterface: public QObject
{

    Q_OBJECT

public:
    using ID = SensorItemInterface::ID;
    using Item = SensorItemInterface;

public:
    using Map = QMap<ID, Item *>;
    using Iterator = Map::Iterator;
    using ConstIterator = Map::ConstIterator;

public:
    SensorMapInterface(QObject * const parent = nullptr) : QObject(parent){}
    virtual ~SensorMapInterface() = default;

public:
    virtual SensorMapInterface::Iterator begin() = 0;
    virtual SensorMapInterface::ConstIterator constBegin() const = 0;

public:
    virtual SensorMapInterface::Iterator end() = 0;
    virtual SensorMapInterface::ConstIterator constEnd() const = 0;

public:
    virtual bool add(SensorMapInterface::ID const & id) = 0;
    virtual bool add(SensorMapInterface::Item * const item) = 0;

public:
    virtual bool remove(SensorMapInterface::ID const & id) = 0;
    virtual bool remove(SensorMapInterface::Item * const item) = 0;

public:
    virtual SensorMapInterface::Item * getItem(SensorMapInterface::ID const & id) const = 0;

public:
    virtual void setID(SensorMapInterface::Item  * const item,
                         SensorMapInterface::ID const & id) = 0;

    virtual SensorMapInterface::ID getID(SensorMapInterface::Item  * const item) const = 0;

    virtual SensorMapInterface::ID generateID() =0;

public:
    virtual bool contains (SensorMapInterface::ID const & id) const =0;
    virtual int count() const =0;

public:
    virtual void clear() =0;

public:
    virtual QList<ID> keys() const =0;
    virtual QList<Item*> values() const =0;
};

#endif // SENSORMAPINTERFACE_H
