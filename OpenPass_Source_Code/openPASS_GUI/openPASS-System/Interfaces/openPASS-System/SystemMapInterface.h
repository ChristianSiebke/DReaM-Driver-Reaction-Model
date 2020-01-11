/****************************************************************************** 
* Copyright (c) 2017 Volkswagen Group of America. 
* 
* This program and the accompanying materials are made 
* available under the terms of the Eclipse Public License 2.0 
* which is available at https://www.eclipse.org/legal/epl-2.0/ 
* 
* SPDX-License-Identifier: EPL-2.0 
******************************************************************************/ 

#ifndef SYSTEMMAPINTERFACE_H
#define SYSTEMMAPINTERFACE_H

#include "openPASS-System/SystemItemInterface.h"
#include <QMap>
#include <QObject>

class SystemMapInterface : public QObject
{
    Q_OBJECT

public:
    using ID = SystemItemInterface::ID;
    using Item = SystemItemInterface;

public:
    using Map = QMap<ID, Item *>;
    using Iterator = Map::Iterator;
    using ConstIterator = Map::ConstIterator;

Q_SIGNALS:
    void cleared();
    void added(SystemItemInterface * const system);
    void modifiedTitle(SystemMapInterface::ID const & id);

public:
    explicit SystemMapInterface(QObject * const parent = nullptr)
        : QObject(parent) {}
    virtual ~SystemMapInterface() = default;

public:
    virtual SystemMapInterface::Iterator begin() = 0;
    virtual SystemMapInterface::ConstIterator begin() const = 0;

public:
    virtual SystemMapInterface::Iterator end() = 0;
    virtual SystemMapInterface::ConstIterator end() const = 0;

public:
    virtual SystemMapInterface::ID getID(SystemMapInterface::Item * const item) const = 0;
    virtual SystemMapInterface::Item * getItem(SystemMapInterface::ID const & id) const = 0;

public:
    virtual SystemMapInterface::ID generateID() = 0;
    virtual SystemItemInterface::Title generateTitle() =0;

public:
    virtual bool add(SystemMapInterface::ID const & id) =0;
    virtual bool add(SystemMapInterface::ID const & id,
                     SystemMapInterface::Item * const system)  = 0;

public:
    virtual bool remove(SystemMapInterface::ID const & id) = 0;

public:
    virtual bool contains(SystemItemInterface::ID const & id) = 0;
    virtual bool contains(SystemItemInterface::Title const & title) = 0;

public:
    virtual int count() const = 0;

public:
    virtual bool clear() = 0;

public:
    virtual QList<Item*> values() const = 0;
};

#endif // SYSTEMMAPINTERFACE_H
