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
//! @file  DriverProfileMapInterface.h
//! @ingroup agentConfigurationPlugin
//! @brief  This class constitutes a container class in which driver profiles
//!         (instances of DriverProfileItemInterface) are registered using their
//!         names as key identifiers. It uses the functionality of Qt' QMap class
//!         but inherits from QObject class in order to utilize Qt's parenting
//!         system as well as signals and slots. Items registered in this
//!         map always become child of this map.
//-----------------------------------------------------------------------------

#ifndef DRIVERPROFILEMAPINTERFACE_H
#define DRIVERPROFILEMAPINTERFACE_H

#include <QObject>

#include "openPASS-AgentConfiguration/DriverProfileItemInterface.h"

//-----------------------------------------------------------------------------
//! @brief  This class constitutes a container class in which driver profiles
//!         (instances of DriverProfileItemInterface) are registered using their
//!         names as key identifiers. It uses the functionality of Qt' QMap class
//!         but inherits from QObject class in order to utilize Qt's parenting
//!         system as well as signals and slots. Items registered in this
//!         map always become child of this map.
//-----------------------------------------------------------------------------
class DriverProfileMapInterface : public QObject
{
    Q_OBJECT

public:
    using Name = DriverProfileItemInterface::Name; //!< typedef for driver profile names (= key identifiers of the map)
    using Item = DriverProfileItemInterface;       //!< typedef for driver profiles (items registered in the map)

public:
    using Map = QMap<Name, Item *>;           //!< typedef for the map, i.e. the container object of driver profiles
    using Iterator = Map::Iterator;           //!< an iterator of the map
    using ConstIterator = Map::ConstIterator; //!< a const iterator of the map (for read-only operations)

public:
    DriverProfileMapInterface(QObject *const parent = nullptr) :
        QObject(parent)
    {
    }
    ~DriverProfileMapInterface() = default;

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal emitted when a driver profile has been added to the map. Transmits
    //! the name of the correponding driver profile.
    //!
    //! @param[in]      name        the name of the added driver profile
    //-----------------------------------------------------------------------------
    void added(Name const &name);

    //-----------------------------------------------------------------------------
    //! Signal emitted when a driver profile has been removed from the map. Transmits
    //! the name of the correponding driver profile.
    //!
    //! @param[in]      name        the name of the removed driver profile
    //-----------------------------------------------------------------------------
    void removed(Name const &name);

    //-----------------------------------------------------------------------------
    //! Signal emitted when the name of the driver profile has been changed. Transmits
    //! both, the new and old names of the profile.
    //!
    //! @param[in]      oldName        the old name of the driver profile
    //! @param[in]      newName        the new name of the driver profile
    //-----------------------------------------------------------------------------
    void modifiedName(Name const &oldName, Name const &newName);

public:
    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing to the first driver profile in the map.
    //!
    //! @return                     The iterator pointing at the first object
    //!                             in the map.
    //-----------------------------------------------------------------------------
    virtual DriverProfileMapInterface::Iterator begin() = 0;

    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing to the first driver profile in the map
    //! (use for read-only since pointing at constant values).
    //!
    //! @return                     The iterator pointing at the first object
    //!                             in the map.
    //-----------------------------------------------------------------------------
    virtual DriverProfileMapInterface::ConstIterator constBegin() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing to the end of the map after the last driver profile.
    //!
    //! @return                     The iterator pointing at the end of the map.
    //-----------------------------------------------------------------------------
    virtual DriverProfileMapInterface::Iterator end() = 0;

    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing at the end of the map after the last driver profile
    //! (use for read-only).
    //!
    //! @return                     The iterator pointing at the end of the map.
    //-----------------------------------------------------------------------------
    virtual DriverProfileMapInterface::ConstIterator constEnd() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which creates a new driver profile (instance of DriverProfileItemInterface) and
    //! adds it to the map under the given name.
    //!
    //! @param[in]      name        the name under which the newly created item is registered
    //!
    //! @return                     "true", if name is not used already, otherwise "false"
    //-----------------------------------------------------------------------------
    virtual bool add(DriverProfileMapInterface::Name const &name) = 0;

    //-----------------------------------------------------------------------------
    //! Method which creates a copy of the given driver profile (seond argument) and
    //! adds it to the map using the given name.
    //!
    //! @param[in]      name        the name under which the copied item is registered
    //! @param[in]      item        the driver pofile to be copied
    //!
    //! @return                     "true", if name is not used already, otherwise "false"
    //-----------------------------------------------------------------------------
    virtual bool add(DriverProfileMapInterface::Name const &name,
                     DriverProfileMapInterface::Item const *const driver) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which removes the driver profile with the given name from the map.
    //! The profile is deleted after removing. Nothing will be done if there is
    //! no such profile.
    //!
    //! @param[in]      name        the name of the driver profile to be removed
    //!
    //! @return                     "true", if name exists in the map, otherwise
    //!                             "false"
    //-----------------------------------------------------------------------------
    virtual bool remove(DriverProfileMapInterface::Name const &name) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns a pointer to the driver profile with the given name.
    //! If the map does not contain a driver profile with this name,
    //! nullptr is returned.
    //!
    //! @param[in]      name        the name of the driver profile to be returned
    //!
    //! @return                     the pointer to a driver profile of given name,
    //!                             nullptr if given name does not exist in the map
    //-----------------------------------------------------------------------------
    virtual DriverProfileMapInterface::Item *getItem(DriverProfileMapInterface::Name const &name) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method wich changes the name of the given driver profile to the one passed
    //! as second argument.
    //!
    //! @param[in]      item        the pointer to the driver profile to be renamed
    //! @param[in]      name        the new name of the agent profile
    //!
    //-----------------------------------------------------------------------------
    virtual void setName(DriverProfileMapInterface::Item *const item,
                         DriverProfileMapInterface::Name const &name) = 0;

    //-----------------------------------------------------------------------------
    //! Method wich returns the name of the given driver profile.
    //!
    //! @param[in]      item        the pointer to the driver profile in question
    //! @return                     the name of the given driver profile
    //-----------------------------------------------------------------------------
    virtual DriverProfileMapInterface::Name getName(DriverProfileMapInterface::Item *const item) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method wich checks whether the map contains a driver profile with the given
    //! name.
    //!
    //! @param[in]      name        the name to be searched for
    //! @return                     true, if profile with given name exists, otherwise false.
    //-----------------------------------------------------------------------------
    virtual bool contains(DriverProfileMapInterface::Name const &name) const = 0;

    //-----------------------------------------------------------------------------
    //! Method which returns the number of driver profiles in the map.
    //!
    //! @return                     the number of driver profiles in the map
    //-----------------------------------------------------------------------------
    virtual int count() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the (string) list of all driver profile names in the map.
    //!
    //! @return                     the (string) list of all driver profile names
    //-----------------------------------------------------------------------------
    virtual QStringList keys() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which returns a list of pointers to all driver profiles in the map.
    //!
    //! @return                     the list of pointers to all driver profiles
    //-----------------------------------------------------------------------------
    virtual QList<Item *> values() const = 0;
};

#endif // DRIVERPROFILEMAPINTERFACE_H
