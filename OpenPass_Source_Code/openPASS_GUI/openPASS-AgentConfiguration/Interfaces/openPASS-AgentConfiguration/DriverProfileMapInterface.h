/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef DRIVERPROFILEMAPINTERFACE_H
#define DRIVERPROFILEMAPINTERFACE_H

#include "openPASS-AgentConfiguration/DriverProfileItemInterface.h"

#include <QObject>

class DriverProfileMapInterface : public QObject
{
    Q_OBJECT

public:
    using Name = DriverProfileItemInterface::Name;
    using Item = DriverProfileItemInterface;

public:
    using Map = QMap<Name, Item *>;
    using Iterator = Map::Iterator;
    using ConstIterator = Map::ConstIterator;

public:
    DriverProfileMapInterface(QObject * const parent = nullptr) : QObject(parent){}
    ~DriverProfileMapInterface() = default;

Q_SIGNALS:
    void added(Name const & name);
    void removed(Name const & name);
    void modifiedName(Name const & oldName, Name const & newName);

public:
    virtual DriverProfileMapInterface::Iterator begin() = 0;
    virtual DriverProfileMapInterface::ConstIterator constBegin() const = 0;

public:
    virtual DriverProfileMapInterface::Iterator end() = 0;
    virtual DriverProfileMapInterface::ConstIterator constEnd() const = 0;

public:
    virtual bool add(DriverProfileMapInterface::Name const & name) = 0;
    virtual bool add(DriverProfileMapInterface::Name const & name,
                     DriverProfileMapInterface::Item const * const driver) = 0;

public:
    virtual bool remove(DriverProfileMapInterface::Name const & name) = 0;
    virtual bool remove(DriverProfileMapInterface::Item * const item) = 0;

public:
    virtual DriverProfileMapInterface::Item * getItem(DriverProfileMapInterface::Name const & name) const = 0;

public:
    virtual void setName(DriverProfileMapInterface::Item  * const item,
                         DriverProfileMapInterface::Name const & name) = 0;

    virtual DriverProfileMapInterface::Name getName(DriverProfileMapInterface::Item  * const item) const = 0;

public:
    virtual bool contains (DriverProfileMapInterface::Name const & name) const =0;
    virtual int count() const =0;

public:
    virtual void clear() =0;

public:
    virtual QStringList keys() const =0;
    virtual QList<Item*> values() const =0;
};

#endif // DRIVERPROFILEMAPINTERFACE_H
