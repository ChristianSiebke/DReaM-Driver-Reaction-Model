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
//! @file  VehicleComponentMapInterface.h
//! @ingroup agentConfigurationPlugin
//! @brief  This class constitutes a container class for vehicle component data
//!         (instances of VehicleComponentItemInterface) using the vehicle
//!         component's  type (i.e. the algorithm it represents) as a key
//!         identifier. This class uses the functionality of Qt' QMap class
//!         but inherits from QObject class in order to utilize Qt's parenting
//!         system. Items registered in this map always become child of this map.
//-----------------------------------------------------------------------------

#ifndef VEHICLECOMPONENTMAPINTERFACE_H
#define VEHICLECOMPONENTMAPINTERFACE_H

#include "openPASS-AgentConfiguration/VehicleComponentItemInterface.h"

#include <QObject>
#include <QMap>

//-----------------------------------------------------------------------------
//! @brief  This class constitutes a container class for vehicle component data
//!         (instances of VehicleComponentItemInterface) using the vehicle
//!         component's  type (i.e. the algorithm it represents) as a key
//!         identifier. This class uses the functionality of Qt' QMap class
//!         but inherits from QObject class in order to utilize Qt's parenting
//!         system. Items registered in this map always become child of this map.
//-----------------------------------------------------------------------------
class VehicleComponentMapInterface: public QObject
{
    Q_OBJECT

public:
    using Type = VehicleComponentItemInterface::Type;           //!< typedef for the vehicle component's type
    using Item = VehicleComponentItemInterface;                 //!< typedef for the vehicle component data

public:
    using Map = QMap<Type, Item *>;                             //!< typedef for the map, i.e. the container object of vehicle component data
    using Iterator = Map::Iterator;                             //!< an iterator of the map
    using ConstIterator = Map::ConstIterator;                   //!< a const iterator of the map (for read-only operations)

public:
    VehicleComponentMapInterface(QObject * const parent = nullptr) : QObject(parent){}
    virtual ~VehicleComponentMapInterface() = default;


public:
    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing to the first vehicle component data in the map.
    //!
    //! @return                     The iterator pointing at the first object
    //!                             in the map.
    //-----------------------------------------------------------------------------
    virtual VehicleComponentMapInterface::Iterator begin() = 0;

    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing to the first vehicle component data object in the
    //! map (use for read-only since pointing at constant values).
    //!
    //! @return                     The iterator pointing at the first object
    //!                             in the map.
    //-----------------------------------------------------------------------------
    virtual VehicleComponentMapInterface::ConstIterator constBegin() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing to the end of the map after the last vehicle component
    //! data object.
    //!
    //! @return                     The iterator pointing at the end of the map.
    //-----------------------------------------------------------------------------
    virtual VehicleComponentMapInterface::Iterator end() = 0;

    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing at the end of the map after the last vehicle component
    //! data object (use for read-only).
    //!
    //! @return                     The iterator pointing at the end of the map.
    //-----------------------------------------------------------------------------
    virtual VehicleComponentMapInterface::ConstIterator constEnd() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which creates a new vehicle component data object (instance of
    //! VehicleComponentItemInterface) and adds it to the map under the given type.
    //!
    //! @param[in]      type        the type under which the newly created item is registered
    //!
    //! @return                     "true", if type is not used already, otherwise "false"
    //-----------------------------------------------------------------------------
    virtual bool add(VehicleComponentMapInterface::Type const & type) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which removes the vehicle component data object with the given type
    //! from the map. The object is deleted after removing. Nothing will be done if there is
    //! no such profile.
    //!
    //! @param[in]      type        the type of the vehicle component data to be removed
    //!
    //! @return                     "true", if type exists in the map, otherwise
    //!                             "false"
    //-----------------------------------------------------------------------------
    virtual bool remove(VehicleComponentMapInterface::Type const & type) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns a pointer to the vehicle component data object of the
    //! given type. If the map does not contain a vehicle component data object of
    //! that type, nullptr is returned.
    //!
    //! @param[in]      type        the type of the vehicle component data object
    //!                             to be returned
    //!
    //! @return                     the pointer to a vehicle component data object of
    //!                             given type, nullptr if no such type exists
    //-----------------------------------------------------------------------------
    virtual VehicleComponentMapInterface::Item * getItem(VehicleComponentMapInterface::Type const & type) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method wich changes the type of the given vehicle component data object
    //! to the one passed as second argument.
    //!
    //! @param[in]      item        the pointer to the vehicle component data object
    //!                             whose type should be changed
    //! @param[in]      type        the new type of the vehicle component data object
    //-----------------------------------------------------------------------------
    virtual void setType(VehicleComponentMapInterface::Item  * const item,
                         VehicleComponentMapInterface::Type const & type) = 0;

    //-----------------------------------------------------------------------------
    //! Method wich returns the type of the given vehicle component data object.
    //!
    //! @param[in]      item        the pointer to the vehicle component data in question
    //! @return                     the type of the given vehicle component data
    //-----------------------------------------------------------------------------
    virtual VehicleComponentMapInterface::Type getType(VehicleComponentMapInterface::Item  * const item) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method wich checks whether the map contains a vehicle component data object
    //! with the given type.
    //!
    //! @param[in]      type        the type to be searched for
    //! @return                     true, if object with given type exists, otherwise false.
    //-----------------------------------------------------------------------------
    virtual bool contains (VehicleComponentMapInterface::Type const & type) const =0;

    //-----------------------------------------------------------------------------
    //! Method which returns the number of vehicle component data objects in the
    //! map.
    //!
    //! @return                     the number of vehicle component data objects
    //!                             in the map
    //-----------------------------------------------------------------------------
    virtual int count() const =0;

public:
    //-----------------------------------------------------------------------------
    //! Method which deletes all registered vehicle component data objects and
    //! clears the map.
    //-----------------------------------------------------------------------------
    virtual void clear() =0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the list of all vehicle component types in the map.
    //!
    //! @return                     the list of all vehicle component data types
    //-----------------------------------------------------------------------------
    virtual QStringList keys() const =0;

    //-----------------------------------------------------------------------------
    //! Method which returns a list of pointers to all vehicle component data objects
    //! in the map.
    //!
    //! @return                     the list of pointers to all vehicle component data
    //!                             objects
    //-----------------------------------------------------------------------------
    virtual QList<Item*> values() const =0;
};

#endif // VEHICLECOMPONENTMAPINTERFACE_H


