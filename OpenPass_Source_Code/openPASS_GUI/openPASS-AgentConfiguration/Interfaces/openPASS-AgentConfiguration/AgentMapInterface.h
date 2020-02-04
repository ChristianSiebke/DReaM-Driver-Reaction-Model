/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  AgentMapInterface.h
//! @ingroup agentConfigurationPlugin
//! @brief  This class constitutes a container class in which agent profiles
//!         (instances of AgentItemInterface) are registered using their names
//!         as key identifiers. It uses the functionality of Qt' QMap class
//!         but inherits from QObject class in order to utilize Qt's parenting
//!         system as well as signals and slots.
//-----------------------------------------------------------------------------
#ifndef AGENTMAPINTERFACE_H
#define AGENTMAPINTERFACE_H

#include "openPASS-AgentConfiguration/AgentItemInterface.h"

#include <QObject>
#include <QMap>

//-----------------------------------------------------------------------------
//! @brief  This class constitutes a container class in which agent profiles
//!         (instances of AgentItemInterface) are registered using their names
//!         as key identifiers. It uses the functionality of Qt' QMap class
//!         but inherits from QObject class in order to utilize Qt's parenting
//!         system as well as signals and slots.
//-----------------------------------------------------------------------------
class AgentMapInterface : public QObject
{
    Q_OBJECT

public:
    using Name = AgentItemInterface::Name;
    using Item = AgentItemInterface;

public:
    using Map = QMap<Name, Item *>;
    using Iterator = Map::Iterator;
    using ConstIterator = Map::ConstIterator;

public:
    explicit AgentMapInterface (QObject * parent = nullptr)
                               : QObject(parent){}
    virtual ~AgentMapInterface() = default;

Q_SIGNALS:
    void added(Name name);
    void removed(Name name);
    void modifiedName(Name oldName, Name newName);
    void cleared();

public:
    virtual AgentMapInterface::Iterator begin() = 0;
    virtual AgentMapInterface::ConstIterator constBegin() const = 0;

public:
    virtual AgentMapInterface::Iterator end() = 0;
    virtual AgentMapInterface::ConstIterator constEnd() const = 0;

public:
    virtual bool add(AgentMapInterface::Name const & name) = 0;
    virtual bool add(AgentMapInterface::Name const &name,  AgentMapInterface::Item const * const item) = 0;

public:
    virtual bool remove(AgentMapInterface::Name const & name) = 0;
    virtual bool remove(AgentMapInterface::Item * const item) = 0;

public:
    virtual AgentMapInterface::Item * getItem(AgentMapInterface::Name const & name) const = 0;

public:
    virtual void setName(AgentMapInterface::Item  * const item,
                         AgentMapInterface::Name const & name) = 0;

    virtual AgentMapInterface::Name getName(AgentMapInterface::Item  * const item) const = 0;

public:
    virtual bool contains (AgentMapInterface::Name const & name) const =0;
    virtual int count() const =0;

public:
    virtual void removeAll() = 0;
    virtual void clear() =0;

public:
    virtual QStringList keys() const =0;
    virtual QList<Item*> values() const =0;

};

#endif // AGENTMAPINTERFACE_H
