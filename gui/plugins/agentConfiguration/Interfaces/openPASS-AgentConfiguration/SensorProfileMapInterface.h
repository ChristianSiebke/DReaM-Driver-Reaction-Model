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
//! @file  SensorProfileMapInterface.h
//! @ingroup agentConfigurationPlugin
//! @brief  This class constitutes a container class in which sensor profiles
//!         (instances of SensorProfileItemInterface) are registered. As a key
//!         identifier the pair of sensor name and type (corrsponding to its
//!         implementation) is used.
//!         This class uses the functionality of Qt' QMap class
//!         but inherits from QObject class in order to utilize Qt's parenting
//!         system. Items registered in this map always become child of this map.
//-----------------------------------------------------------------------------

#ifndef SENSORPROFILEMAPINTERFACE_H
#define SENSORPROFILEMAPINTERFACE_H

#include <QMap>
#include <QObject>
#include <QPair>

#include "openPASS-AgentConfiguration/SensorProfileItemInterface.h"

//-----------------------------------------------------------------------------
//! @brief  This class constitutes a container class in which sensor profiles
//!         (instances of SensorProfileItemInterface) are registered. As a key
//!         identifier the pair of sensor name and type (corrsponding to its
//!         implementation) is used.
//!         This class uses the functionality of Qt' QMap class
//!         but inherits from QObject class in order to utilize Qt's parenting
//!         system. Items registered in this  map always become child of this map.
//-----------------------------------------------------------------------------
class SensorProfileMapInterface : public QObject
{
    Q_OBJECT

public:
    using Name = SensorProfileItemInterface::Name; //!< typedef for the sensor profiles name
    using Type = SensorProfileItemInterface::Type; //!< typedef for the sensor type (i.e. its algorithm/implementation)
    using ID = QPair<Name, Type>;                  //!< typedef for the pair of name and type used as an Id
    using Item = SensorProfileItemInterface;       //!< typedef for the sensor profile

public:
    using Map = QMap<ID, Item *>;             //!< typedef for the map, i.e. the container object of sensor profiles
    using Iterator = Map::Iterator;           //!< an iterator of the map
    using ConstIterator = Map::ConstIterator; //!< a const iterator of the map (for read-only operations)

public:
    SensorProfileMapInterface(QObject *const parent = nullptr) :
        QObject(parent)
    {
    }
    virtual ~SensorProfileMapInterface() = default;

public:
    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing to the first sensor profile in the map.
    //!
    //! @return                     The iterator pointing at the first object
    //!                             in the map.
    //-----------------------------------------------------------------------------
    virtual SensorProfileMapInterface::Iterator begin() = 0;

    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing to the first sensor profile in the map
    //! (use for read-only since pointing at constant values).
    //!
    //! @return                     The iterator pointing at the first object
    //!                             in the map.
    //-----------------------------------------------------------------------------
    virtual SensorProfileMapInterface::ConstIterator constBegin() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing to the end of the map after the last sensor profile.
    //!
    //! @return                     The iterator pointing at the end of the map.
    //-----------------------------------------------------------------------------
    virtual SensorProfileMapInterface::Iterator end() = 0;

    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing at the end of the map after the last sensor profile
    //! (use for read-only).
    //!
    //! @return                     The iterator pointing at the end of the map.
    //-----------------------------------------------------------------------------
    virtual SensorProfileMapInterface::ConstIterator constEnd() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which creates a new sensor profile (instance of SensorProfileItemInterface) and
    //! adds it to the map under the given Id.
    //!
    //! @param[in]      Id        the Id under which the newly created item is registered
    //!
    //! @return                     "true", if Id is not used already, otherwise "false"
    //-----------------------------------------------------------------------------
    virtual bool add(SensorProfileMapInterface::ID const &id) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which removes the sensor profile with the given Id from the map.
    //! The profile is deleted after removing. Nothing will be done if there is
    //! no such profile.
    //!
    //! @param[in]      Id        the Id of the sensor profile to be removed
    //!
    //! @return                     "true", if Id exists in the map, otherwise
    //!                             "false"
    //-----------------------------------------------------------------------------
    virtual bool remove(SensorProfileMapInterface::ID const &id) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns a pointer to the sensor profile with the given Id.
    //! If the map does not contain a sensor profile with this Id,
    //! nullptr is returned.
    //!
    //! @param[in]      Id        the Id of the sensor profile to be returned
    //!
    //! @return                     the pointer to a sensor profile of given Id,
    //!                             nullptr if given Id does not exist in the map
    //-----------------------------------------------------------------------------
    virtual SensorProfileMapInterface::Item *getItem(SensorProfileMapInterface::ID const &id) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method wich changes the Id of the given sensor profile to the one passed
    //! as second argument.
    //!
    //! @param[in]      item        the pointer to the sensor profile whose id should
    //!                             be changed
    //! @param[in]      Id          the new Id of the sensor profile
    //-----------------------------------------------------------------------------
    virtual void setID(SensorProfileMapInterface::Item *const item,
                       SensorProfileMapInterface::ID const &id) = 0;

    //-----------------------------------------------------------------------------
    //! Method wich returns the Id of the given sensor profile.
    //!
    //! @param[in]      item        the pointer to the sensor profile in question
    //! @return                     the Id of the given sensor profile
    //-----------------------------------------------------------------------------
    virtual SensorProfileMapInterface::ID getID(SensorProfileMapInterface::Item *const item) const = 0;

    //-----------------------------------------------------------------------------
    //! Method wich generates a default name of the pattern "Standard %1" for a sensor
    //! profile of the given type. The number  %1 is chosen such that the generated
    //! name is not already used by another sensor profile of the same type.
    //!
    //! @param[in]      type        the sensor type for which a name should be generated
    //! @return                     the generated default name
    //-----------------------------------------------------------------------------
    virtual Name generateName(Type type) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method wich checks whether the map contains a sensor profile with the given
    //! Id.
    //!
    //! @param[in]      Id        the Id to be searched for
    //! @return                     true, if profile with given Id exists, otherwise false.
    //-----------------------------------------------------------------------------
    virtual bool contains(SensorProfileMapInterface::ID const &id) const = 0;

    //-----------------------------------------------------------------------------
    //! Method which returns the number of sensor profiles in the map.
    //!
    //! @return                     the number of sensor profiles in the map
    //-----------------------------------------------------------------------------
    virtual int count() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which deletes all registered sensor profiles and clears the map.
    //-----------------------------------------------------------------------------
    virtual void clear() = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the list of all sensor profile Ids in the map.
    //!
    //! @return                     the list of all sensor profile Ids
    //-----------------------------------------------------------------------------
    virtual QList<ID> keys() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which returns a list of pointers to all sensor profiles in the map.
    //!
    //! @return                     the list of pointers to all sensor profiles
    //-----------------------------------------------------------------------------
    virtual QList<Item *> values() const = 0;
};

#endif // SENSORPROFILEMAPINTERFACE_H
