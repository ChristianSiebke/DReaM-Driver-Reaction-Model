/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef SENSORPROFILEPARAMETERMAPINTERFACE_H
#define SENSORPROFILEPARAMETERMAPINTERFACE_H

#include <QMap>
#include <QObject>

#include "openPASS-AgentConfiguration/SensorProfileParameterItemInterface.h"

class SensorProfileParameterMapInterface : public QObject
{
    Q_OBJECT
public:
    using ID = SensorProfileParameterItemInterface::ID;
    using Item = SensorProfileParameterItemInterface;

public:
    using Map = QMap<ID, Item *>;
    using Iterator = Map::Iterator;
    using ConstIterator = Map::ConstIterator;

public:
    SensorProfileParameterMapInterface(QObject *const parent = nullptr) :
        QObject(parent)
    {
    }
    virtual ~SensorProfileParameterMapInterface() = default;

public:
    virtual SensorProfileParameterMapInterface::Iterator begin() = 0;
    virtual SensorProfileParameterMapInterface::ConstIterator constBegin() const = 0;

public:
    virtual SensorProfileParameterMapInterface::Iterator end() = 0;
    virtual SensorProfileParameterMapInterface::ConstIterator constEnd() const = 0;

public:
    virtual bool add(SensorProfileParameterMapInterface::ID const &id) = 0;
    virtual bool add(SensorProfileParameterMapInterface::Item *const item) = 0;

public:
    virtual bool remove(SensorProfileParameterMapInterface::ID const &id) = 0;
    virtual bool remove(SensorProfileParameterMapInterface::Item *const item) = 0;

public:
    virtual SensorProfileParameterMapInterface::Item *getItem(SensorProfileParameterMapInterface::ID const &id) const = 0;

public:
    virtual void setID(SensorProfileParameterMapInterface::Item *const item,
                       SensorProfileParameterMapInterface::ID const &id) = 0;

    virtual SensorProfileParameterMapInterface::ID getID(SensorProfileParameterMapInterface::Item *const item) const = 0;

    virtual SensorProfileParameterMapInterface::ID generateID() = 0;

public:
    virtual bool contains(SensorProfileParameterMapInterface::ID const &id) const = 0;
    virtual int count() const = 0;

public:
    virtual void clear() = 0;

public:
    virtual QList<ID> keys() const = 0;
    virtual QList<Item *> values() const = 0;
};

#endif // SENSORPROFILEPARAMETERMAPINTERFACE_H
