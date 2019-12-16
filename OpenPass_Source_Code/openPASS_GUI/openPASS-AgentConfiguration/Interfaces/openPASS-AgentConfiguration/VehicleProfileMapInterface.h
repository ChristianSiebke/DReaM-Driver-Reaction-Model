/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef VEHICLEPROFILEMAPINTERFACE_H
#define VEHICLEPROFILEMAPINTERFACE_H

#include "openPASS-AgentConfiguration/VehicleProfileItemInterface.h"

#include <QObject>
#include <QMap>

class VehicleProfileMapInterface : public QObject
{
    Q_OBJECT

public:
    using Name = VehicleProfileItemInterface::Name;
    using Item = VehicleProfileItemInterface;

public:
    using Map = QMap<Name, Item *>;
    using Iterator = Map::Iterator;
    using ConstIterator = Map::ConstIterator;

public:
    explicit VehicleProfileMapInterface(QObject * const parent = nullptr) : QObject(parent){}
    virtual ~VehicleProfileMapInterface() = default;


Q_SIGNALS:
    void removed(Name const & name);
    void added(Name const & name);
    void modifiedName(Name const & oldName, Name const & newName);

public:
    virtual VehicleProfileMapInterface::Iterator begin() = 0;
    virtual VehicleProfileMapInterface::ConstIterator constBegin() const = 0;

public:
    virtual VehicleProfileMapInterface::Iterator end() = 0;
    virtual VehicleProfileMapInterface::ConstIterator constEnd() const = 0;

public:
    virtual bool add(VehicleProfileMapInterface::Name const & name) = 0;
    virtual bool add(VehicleProfileMapInterface::Name const & name,
                     VehicleProfileMapInterface::Item const * const item) = 0;

public:
    virtual bool remove(VehicleProfileMapInterface::Name const & name) = 0;
    virtual bool remove(VehicleProfileMapInterface::Item * const item) = 0;

public:
    virtual VehicleProfileMapInterface::Item * getItem(VehicleProfileMapInterface::Name const & name) const = 0;

public:
    virtual void setName(VehicleProfileMapInterface::Item  * const item,
                         VehicleProfileMapInterface::Name const & name) = 0;

    virtual VehicleProfileMapInterface::Name getName(VehicleProfileMapInterface::Item  * const item) const = 0;

public:
    virtual bool contains (VehicleProfileMapInterface::Name const & name) const =0;
    virtual int count() const =0;

public:
    virtual void clear() =0;

public:
    virtual QStringList keys() const =0;
    virtual QList<Item*> values() const =0;
};


#endif // VEHICLEPROFILEMAPINTERFACE_H
