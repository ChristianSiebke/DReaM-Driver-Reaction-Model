/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  VehicleComponentProfileMapInterface.h
//! @ingroup agentConfigurationPlugin
//! @brief  This class constitutes a container class for vehicle component
//!         profiles (instances of VehicleComponentProfileItemInterface). As a key
//!         identifier the pair of the profile's name and type (corrsponding to to
//!         the implementation/algorithm of the vehicle component) is used.
//!         This class uses the functionality of Qt' QMap class
//!         but inherits from QObject class in order to utilize Qt's parenting
//!         system. Items registered in this map always become child of this map.
//-----------------------------------------------------------------------------

#ifndef VEHICLECOMPONENTPROFILEMAPINTERFACE_H
#define VEHICLECOMPONENTPROFILEMAPINTERFACE_H

#include <QMap>
#include <QObject>
#include <QPair>

#include "openPASS-AgentConfiguration/VehicleComponentProfileItemInterface.h"

//-----------------------------------------------------------------------------
//! @brief  This class constitutes a container class for vehicle component
//!         profiles (instances of VehicleComponentProfileItemInterface). As a key
//!         identifier the pair of the profile's name and type (corrsponding to to
//!         the implementation/algorithm of the vehicle component) is used.
//!         This class uses the functionality of Qt' QMap class
//!         but inherits from QObject class in order to utilize Qt's parenting
//!         system. Items registered in this map always become child of this map.
//-----------------------------------------------------------------------------
class VehicleComponentProfileMapInterface : public QObject
{
    Q_OBJECT

public:
    using Name = VehicleComponentProfileItemInterface::Name; //!< typedef for the vehicle component profiles name
    using Type = VehicleComponentProfileItemInterface::Type; //!< typedef for the sensor type (i.e. its algorithm/implementation)
    using ID = QPair<Name, Type>;                            //!< typedef for the pair of name and type used as an Id
    using Item = VehicleComponentProfileItemInterface;       //!< typedef for the vehicle component profile

public:
    using Map = QMap<ID, Item *>;             //!< typedef for the map, i.e. the container object of vehicle component profiles
    using Iterator = Map::Iterator;           //!< an iterator of the map
    using ConstIterator = Map::ConstIterator; //!< a const iterator of the map (for read-only operations)

public:
    VehicleComponentProfileMapInterface(QObject *const parent = nullptr) :
        QObject(parent)
    {
    }
    virtual ~VehicleComponentProfileMapInterface() = default;

public:
    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing to the first vehicle component profile in the map.
    //!
    //! @return                     The iterator pointing at the first object
    //!                             in the map.
    //-----------------------------------------------------------------------------
    virtual VehicleComponentProfileMapInterface::Iterator begin() = 0;

    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing to the first vehicle component profile in the map
    //! (use for read-only since pointing at constant values).
    //!
    //! @return                     The iterator pointing at the first object
    //!                             in the map.
    //-----------------------------------------------------------------------------
    virtual VehicleComponentProfileMapInterface::ConstIterator constBegin() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing to the end of the map after the last vehicle component profile.
    //!
    //! @return                     The iterator pointing at the end of the map.
    //-----------------------------------------------------------------------------
    virtual VehicleComponentProfileMapInterface::Iterator end() = 0;

    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing at the end of the map after the last vehicle component profile
    //! (use for read-only).
    //!
    //! @return                     The iterator pointing at the end of the map.
    //-----------------------------------------------------------------------------
    virtual VehicleComponentProfileMapInterface::ConstIterator constEnd() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which creates a new vehicle component profile (instance of
    //! VehicleComponentProfileItemInterface) and adds it to the map under the given Id.
    //!
    //! @param[in]      Id        the Id under which the newly created item is registered
    //!
    //! @return                   "true", if Id is not used already, otherwise "false"
    //-----------------------------------------------------------------------------
    virtual bool add(VehicleComponentProfileMapInterface::ID const &id) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which removes the vehicle component profile with the given Id from the map.
    //! The profile is deleted after removing. Nothing will be done if there is
    //! no such profile.
    //!
    //! @param[in]      Id        the Id of the vehicle component profile to be removed
    //!
    //! @return                     "true", if Id exists in the map, otherwise
    //!                             "false"
    //-----------------------------------------------------------------------------
    virtual bool remove(VehicleComponentProfileMapInterface::ID const &id) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns a pointer to the vehicle component profile with the given Id.
    //! If the map does not contain a vehicle component profile with this Id,
    //! nullptr is returned.
    //!
    //! @param[in]      Id        the Id of the vehicle component profile to be returned
    //!
    //! @return                     the pointer to a vehicle component profile of given Id,
    //!                             nullptr if given Id does not exist in the map
    //-----------------------------------------------------------------------------
    virtual VehicleComponentProfileMapInterface::Item *getItem(VehicleComponentProfileMapInterface::ID const &id) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method wich changes the Id of the given vehicle component profile to the one passed
    //! as second argument.
    //!
    //! @param[in]      item        the pointer to the vehicle component profile whose id should
    //!                             be changed
    //! @param[in]      Id          the new Id of the vehicle component profile
    //-----------------------------------------------------------------------------
    virtual void setID(VehicleComponentProfileMapInterface::Item *const item,
                       VehicleComponentProfileMapInterface::ID const &id) = 0;

    //-----------------------------------------------------------------------------
    //! Method wich returns the Id of the given vehicle component profile.
    //!
    //! @param[in]      item        the pointer to the vehicle component profile in question
    //! @return                     the Id of the given vehicle component profile
    //-----------------------------------------------------------------------------
    virtual VehicleComponentProfileMapInterface::ID getID(VehicleComponentProfileMapInterface::Item *const item) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method wich checks whether the map contains a vehicle component profile with the given
    //! Id.
    //!
    //! @param[in]      Id        the Id to be searched for
    //! @return                     true, if profile with given Id exists, otherwise false.
    //-----------------------------------------------------------------------------
    virtual bool contains(VehicleComponentProfileMapInterface::ID const &id) const = 0;

    //-----------------------------------------------------------------------------
    //! Method which returns the number of vehicle component profiles in the map.
    //!
    //! @return                     the number of vehicle component profiles in the map
    //-----------------------------------------------------------------------------
    virtual int count() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which deletes all registered vehicle component profiles and clears the map.
    //-----------------------------------------------------------------------------
    virtual void clear() = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the list of all vehicle component profile Ids in the map.
    //!
    //! @return                     the list of all vehicle component profile Ids
    //-----------------------------------------------------------------------------
    virtual QList<ID> keys() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which returns a list of pointers to all vehicle component profiles in the map.
    //!
    //! @return                     the list of pointers to all vehicle component profiles
    //-----------------------------------------------------------------------------
    virtual QList<Item *> values() const = 0;
};

#endif // VEHICLECOMPONENTPROFILEMAPINTERFACE_H
