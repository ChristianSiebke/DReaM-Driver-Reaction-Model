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
//!         system as well as signals and slots. Items registered in this
//!         map always become child of this map.
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
//!         system as well as signals and slots. Items registered in this
//!         map always become child of this map.
//-----------------------------------------------------------------------------
class AgentMapInterface : public QObject
{
    Q_OBJECT

public:
    using Name = AgentItemInterface::Name;      //!< the name of the agent profile (key identifier)
    using Item = AgentItemInterface;            //!< the interface to the agent profiles

public:
    using Map = QMap<Name, Item *>;             //!< the map (container class) for registering agent profiles
    using Iterator = Map::Iterator;             //!< an iterator for searching within the map
    using ConstIterator = Map::ConstIterator;   //!< a constant iterator for read-only operations

public:
    explicit AgentMapInterface (QObject * parent = nullptr)
                               : QObject(parent){}
    virtual ~AgentMapInterface() = default;

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal emitted when an agent profile has been added to the map. Transmits
    //! the name of the correponding agent profile.
    //!
    //! @param[in]      name        the name of the added agent profile
    //-----------------------------------------------------------------------------
    void added(Name name);

    //-----------------------------------------------------------------------------
    //! Signal emitted when an agent profile has been removed from the map. Transmits
    //! the name of the correponding agent profile.
    //!
    //! @param[in]      name        the name of the removed agent profile
    //-----------------------------------------------------------------------------
    void removed(Name name);

    //-----------------------------------------------------------------------------
    //! Signal emitted when the name of the agent profile has been changed. Transmits
    //! both, the new and old names of the profile.
    //!
    //! @param[in]      oldName        the old name of the agent profile
    //! @param[in]      newName        the new name of the agent profile
    //-----------------------------------------------------------------------------
    void modifiedName(Name oldName, Name newName);

    //-----------------------------------------------------------------------------
    //! Signal emitted when the map has been cleared.
    //-----------------------------------------------------------------------------
    void cleared();

public:
    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing to the first agent profile in the map.
    //!
    //! @return                     The iterator pointing at the first object
    //!                             in the map.
    //-----------------------------------------------------------------------------
    virtual AgentMapInterface::Iterator begin() = 0;

    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing to the first agent profile in the map
    //! (use for read-only since pointing at constant values).
    //!
    //! @return                     The iterator pointing at the first object
    //!                             in the map.
    //-----------------------------------------------------------------------------
    virtual AgentMapInterface::ConstIterator constBegin() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing to the end of the map after the last agent profile.
    //!
    //! @return                     The iterator pointing at the end of the map.
    //-----------------------------------------------------------------------------
    virtual AgentMapInterface::Iterator end() = 0;

    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing at the end of the map after the last agent profile
    //! (use for read-only).
    //!
    //! @return                     The iterator pointing at the end of the map.
    //-----------------------------------------------------------------------------
    virtual AgentMapInterface::ConstIterator constEnd() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which creates a new agent profile (instance of AgentItemInterface) and
    //! adds it to the map under the given name.
    //!
    //! @param[in]      name        the name under which the newly created item is registered
    //!
    //! @return                     "true", if name is not used already, otherwise "false"
    //-----------------------------------------------------------------------------
    virtual bool add(AgentMapInterface::Name const & name) = 0;

    //-----------------------------------------------------------------------------
    //! Method which creates a copy of the given agent profile and adds it to the map
    //! using the given name.
    //!
    //! @param[in]      name        the name under which the copied item is registered
    //! @param[in]      item        the agent pofile to be copied
    //!
    //! @return                     "true", if name is not used already, otherwise "false"
    //-----------------------------------------------------------------------------
    virtual bool add(AgentMapInterface::Name const &name,
                     AgentMapInterface::Item const * const item) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which removes the agent profile with the given name from the map.
    //! The profile is deleted after removing. Nothing will be done if there is no
    //! such profile.
    //!
    //! @param[in]      name        the name of the agent profile to be removed
    //!
    //! @return                     "true", if name exists in the map, otherwise
    //!                             "false"
    //-----------------------------------------------------------------------------
    virtual bool remove(AgentMapInterface::Name const & name) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns a pointer to the agent profile with the given name.
    //! If the map does not contain an agent profile with the given name,
    //! nullptr is returned.
    //!
    //! @param[in]      name        the name of the agent profile to be returned
    //!
    //! @return                     the pointer to an agent profile with given name,
    //!                             nullptr if given name does not exist in the map
    //-----------------------------------------------------------------------------
    virtual AgentMapInterface::Item * getItem(AgentMapInterface::Name const & name) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method wich changes the name of the given agent profile to the one passed
    //! as second argument.
    //!
    //! @param[in]      item        the pointer to the agent profile to be renamed
    //! @param[in]      name        the new name of the agent profile
    //!
    //-----------------------------------------------------------------------------
    virtual void setName(AgentMapInterface::Item  * const item,
                         AgentMapInterface::Name const & name) = 0;

    //-----------------------------------------------------------------------------
    //! Method wich returns the name of the given agent profile.
    //!
    //! @param[in]      item        the pointer to the agent profile in question
    //! @return                     the name of the given agent profile
    //-----------------------------------------------------------------------------
    virtual AgentMapInterface::Name getName(AgentMapInterface::Item  * const item) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method wich checks whether the map contains an agent profile with the given
    //! name.
    //!
    //! @param[in]      name        the name to be searched for
    //! @return                     true, if profile with given name exists, otherwise false.
    //-----------------------------------------------------------------------------
    virtual bool contains (AgentMapInterface::Name const & name) const =0;

    //-----------------------------------------------------------------------------
    //! Method which returns the number of registered agent profiles.
    //!
    //! @return                     the number of registered agent profiles
    //-----------------------------------------------------------------------------
    virtual int count() const =0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the (string) list of all agent profile names in the map.
    //!
    //! @return                     the (string) list of all agent profile names
    //-----------------------------------------------------------------------------
    virtual QStringList keys() const =0;

    //-----------------------------------------------------------------------------
    //! Method which returns a list of pointers to all agent profiles in the map.
    //!
    //! @return                     the list of pointers to all agent profiles
    //-----------------------------------------------------------------------------
    virtual QList<Item*> values() const =0;

};

#endif // AGENTMAPINTERFACE_H
