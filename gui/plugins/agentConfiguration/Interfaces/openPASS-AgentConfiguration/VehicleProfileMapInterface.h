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
//! @file  VehicleProfileMapInterface.h
//! @ingroup agentConfigurationPlugin
//! @brief  This class constitutes a container class in which vehicle profiles
//!         (instances of VehicleProfileItemInterface) are registered using their
//!         names as key identifiers. It uses the functionality of Qt' QMap class
//!         but inherits from QObject class in order to utilize Qt's parenting
//!         system as well as signals and slots. Items registered in this
//!         map always become child of this map.
//-----------------------------------------------------------------------------

#ifndef VEHICLEPROFILEMAPINTERFACE_H
#define VEHICLEPROFILEMAPINTERFACE_H

#include <QMap>
#include <QObject>

#include "openPASS-AgentConfiguration/VehicleProfileItemInterface.h"

//-----------------------------------------------------------------------------
//! @brief  This class constitutes a container class in which vehicle profiles
//!         (instances of VehicleProfileItemInterface) are registered using their
//!         names as key identifiers. It uses the functionality of Qt' QMap class
//!         but inherits from QObject class in order to utilize Qt's parenting
//!         system as well as signals and slots. Items registered in this
//!         map always become child of this map.
//-----------------------------------------------------------------------------
class VehicleProfileMapInterface : public QObject
{
    Q_OBJECT

public:
    using Name = VehicleProfileItemInterface::Name; //!< typedef for vehicle profile names (= key identifiers of the map)
    using Item = VehicleProfileItemInterface;       //!< typedef for vehicle profiles (items registered in the map)

public:
    using Map = QMap<Name, Item *>;           //!< typedef for the map, i.e. the container object of vehicle profiles
    using Iterator = Map::Iterator;           //!< an iterator of the map
    using ConstIterator = Map::ConstIterator; //!< a const iterator of the map (for read-only operations)

public:
    explicit VehicleProfileMapInterface(QObject *const parent = nullptr) :
        QObject(parent)
    {
    }
    virtual ~VehicleProfileMapInterface() = default;

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal emitted when a vehicle profile has been removed from the map. Transmits
    //! the name of the correponding vehicle profile.
    //!
    //! @param[in]      name        the name of the removed vehicle profile
    //-----------------------------------------------------------------------------
    void removed(Name const &name);

    //-----------------------------------------------------------------------------
    //! Signal emitted when a vehicle profile has been added to the map. Transmits
    //! the name of the correponding vehicle profile.
    //!
    //! @param[in]      name        the name of the added vehicle profile
    //-----------------------------------------------------------------------------
    void added(Name const &name);

    //-----------------------------------------------------------------------------
    //! Signal emitted when the name of the vehicle profile has been changed. Transmits
    //! both, the new and old names of the profile.
    //!
    //! @param[in]      oldName        the old name of the vehicle profile
    //! @param[in]      newName        the new name of the vehicle profile
    //-----------------------------------------------------------------------------
    void modifiedName(Name const &oldName, Name const &newName);

public:
    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing to the first vehicle profile in the map.
    //!
    //! @return                     The iterator pointing at the first object
    //!                             in the map.
    //-----------------------------------------------------------------------------
    virtual VehicleProfileMapInterface::Iterator begin() = 0;

    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing to the first vehicle profile in the map
    //! (use for read-only since pointing at constant values).
    //!
    //! @return                     The iterator pointing at the first object
    //!                             in the map.
    //-----------------------------------------------------------------------------
    virtual VehicleProfileMapInterface::ConstIterator constBegin() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing to the end of the map after the last vehicle profile.
    //!
    //! @return                     The iterator pointing at the end of the map.
    //-----------------------------------------------------------------------------
    virtual VehicleProfileMapInterface::Iterator end() = 0;

    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing at the end of the map after the last vehicle profile
    //! (use for read-only).
    //!
    //! @return                     The iterator pointing at the end of the map.
    //-----------------------------------------------------------------------------
    virtual VehicleProfileMapInterface::ConstIterator constEnd() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which creates a new vehicle profile (instance of VehicleProfileItemInterface) and
    //! adds it to the map under the given name.
    //!
    //! @param[in]      name        the name under which the newly created item is registered
    //!
    //! @return                     "true", if name is not used already, otherwise "false"
    //-----------------------------------------------------------------------------
    virtual bool add(VehicleProfileMapInterface::Name const &name) = 0;

    //-----------------------------------------------------------------------------
    //! Method which creates a copy of the given vehicle profile (seond argument) and
    //! adds it to the map using the given name.
    //!
    //! @param[in]      name        the name under which the copied item is registered
    //! @param[in]      item        the vehicle pofile to be copied
    //!
    //! @return                     "true", if name is not used already, otherwise "false"
    //-----------------------------------------------------------------------------
    virtual bool add(VehicleProfileMapInterface::Name const &name,
                     VehicleProfileMapInterface::Item const *const item) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which removes the vehicle profile with the given name from the map.
    //! The profile is deleted after removing. Nothing will be done if there is
    //! no such profile.
    //!
    //! @param[in]      name        the name of the vehicle profile to be removed
    //!
    //! @return                     "true", if name exists in the map, otherwise
    //!                             "false"
    //-----------------------------------------------------------------------------
    virtual bool remove(VehicleProfileMapInterface::Name const &name) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns a pointer to the vehicle profile with the given name.
    //! If the map does not contain a vehicle profile with this name,
    //! nullptr is returned.
    //!
    //! @param[in]      name        the name of the vehicle profile to be returned
    //!
    //! @return                     the pointer to a vehicle profile of given name,
    //!                             nullptr if given name does not exist in the map
    //-----------------------------------------------------------------------------
    virtual VehicleProfileMapInterface::Item *getItem(VehicleProfileMapInterface::Name const &name) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method wich changes the name of the given vehicle profile to the one passed
    //! as second argument.
    //!
    //! @param[in]      item        the pointer to the vehicle profile to be renamed
    //! @param[in]      name        the new name of the agent profile
    //!
    //-----------------------------------------------------------------------------
    virtual void setName(VehicleProfileMapInterface::Item *const item,
                         VehicleProfileMapInterface::Name const &name) = 0;

    //-----------------------------------------------------------------------------
    //! Method wich returns the name of the given vehicle profile.
    //!
    //! @param[in]      item        the pointer to the vehicle profile in question
    //! @return                     the name of the given vehicle profile
    //-----------------------------------------------------------------------------
    virtual VehicleProfileMapInterface::Name getName(VehicleProfileMapInterface::Item *const item) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method wich checks whether the map contains a vehicle profile with the given
    //! name.
    //!
    //! @param[in]      name        the name to be searched for
    //! @return                     true, if profile with given name exists, otherwise false.
    //-----------------------------------------------------------------------------
    virtual bool contains(VehicleProfileMapInterface::Name const &name) const = 0;

    //-----------------------------------------------------------------------------
    //! Method which returns the number of vehicle profiles in the map.
    //!
    //! @return                     the number of vehicle profiles in the map
    //-----------------------------------------------------------------------------
    virtual int count() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the (string) list of all vehicle profile names in the map.
    //!
    //! @return                     the (string) list of all vehicle profile names
    //-----------------------------------------------------------------------------
    virtual QStringList keys() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which returns a list of pointers to all vehicle profiles in the map.
    //!
    //! @return                     the list of pointers to all vehicle profiles
    //-----------------------------------------------------------------------------
    virtual QList<Item *> values() const = 0;
};

#endif // VEHICLEPROFILEMAPINTERFACE_H
