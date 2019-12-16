/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef PARAMETERMAPINTERFACE_H
#define PARAMETERMAPINTERFACE_H

#include "openPASS-AgentConfiguration/ParameterItemInterface.h"

#include <QObject>
#include <QMap>

class ParameterMapInterface: public QObject
{

    Q_OBJECT

public:
    using ID = ParameterItemInterface::ID;
    using Item = ParameterItemInterface;

public:
    using Map = QMap<ID, Item *>;
    using Iterator = Map::Iterator;
    using ConstIterator = Map::ConstIterator;

public:
    ParameterMapInterface(QObject * const parent = nullptr) : QObject(parent){}
    virtual ~ParameterMapInterface() = default;

public:
    virtual ParameterMapInterface::Iterator begin() = 0;
    virtual ParameterMapInterface::ConstIterator constBegin() const = 0;

public:
    virtual ParameterMapInterface::Iterator end() = 0;
    virtual ParameterMapInterface::ConstIterator constEnd() const = 0;

public:
    virtual bool add(ParameterMapInterface::ID const & id) = 0;
    virtual bool add(ParameterMapInterface::ID const & id, ParameterMapInterface::Item const * const item) = 0;
    virtual bool add(ParameterMapInterface::Item * const item) = 0;

public:
    virtual bool remove(ParameterMapInterface::ID const & id) = 0;
    virtual bool remove(ParameterMapInterface::Item * const item) = 0;

public:
    virtual ParameterMapInterface::Item * getItem(ParameterMapInterface::ID const & id) const = 0;

public:
    virtual void setID(ParameterMapInterface::Item  * const item,
                         ParameterMapInterface::ID const & id) = 0;

    virtual ParameterMapInterface::ID getID(ParameterMapInterface::Item  * const item) const = 0;

    virtual ParameterMapInterface::ID generateID() = 0;

public:
    virtual bool contains (ParameterMapInterface::ID const & id) const =0;
    virtual int count() const =0;

public:
    virtual void clear() =0;

public:
    virtual QList<ID> keys() const =0;
    virtual QList<Item*> values() const =0;
};

#endif //PARAMETERMAPINTERFACE_H
