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
//! @file  SensorMapInterface.h
//! @ingroup agentConfigurationPlugin
//! @brief  This class constitutes a container class in which sensor data
//!         (instances of SensorItemInterface) are registered. It uses the
//!         functionality of Qt's QMap class but inherits from QObject class
//!         in order to utilize Qt's parenting system. Items registered in this
//!         map always become child of this map.
//-----------------------------------------------------------------------------

#ifndef SENSORMAPINTERFACE_H
#define SENSORMAPINTERFACE_H

#include <QMap>
#include <QObject>

#include "openPASS-AgentConfiguration/SensorItemInterface.h"

//-----------------------------------------------------------------------------
//! @brief  This class constitutes a container class in which sensor data
//!         (instances of SensorItemInterface) are registered. It uses the
//!         functionality of Qt's QMap class but inherits from QObject class
//!         in order to utilize Qt's parenting system. Items registered in this
//!         map always become child of this map.
//-----------------------------------------------------------------------------
class SensorMapInterface : public QObject
{
    Q_OBJECT

public:
    using ID = SensorItemInterface::ID; //!< typedef for the id of the sensor data object
    using Item = SensorItemInterface;   //!< typedef for the sensor data object

public:
    using Map = QMap<ID, Item *>;             //!< typedef for the map of sensor data
    using Iterator = Map::Iterator;           //!< typedef for an iterator
    using ConstIterator = Map::ConstIterator; //!< typedef for a constant iterator (read-only operations)

public:
    SensorMapInterface(QObject *const parent = nullptr) :
        QObject(parent)
    {
    }
    virtual ~SensorMapInterface() = default;

public:
    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing to the first sensor data object in the map.
    //!
    //! @return                     The iterator pointing at the first object
    //!                             in the map.
    //-----------------------------------------------------------------------------
    virtual SensorMapInterface::Iterator begin() = 0;

    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing to the first sensor data object in the map
    //! (use for read-only since pointing at constant values).
    //!
    //! @return                     The iterator pointing at the first object
    //!                             in the map.
    //-----------------------------------------------------------------------------
    virtual SensorMapInterface::ConstIterator constBegin() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing to the end of the map after the last dsensor data object.
    //!
    //! @return                     The iterator pointing at the end of the map.
    //-----------------------------------------------------------------------------
    virtual SensorMapInterface::Iterator end() = 0;

    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing at the end of the map after the last sensor data object
    //! (use for read-only).
    //!
    //! @return                     The iterator pointing at the end of the map.
    //-----------------------------------------------------------------------------
    virtual SensorMapInterface::ConstIterator constEnd() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which creates a sensor data object (instance of SensorItemInterface) and
    //! adds it to the map under the given id.
    //!
    //! @param[in]      id        the id under which the newly created item is registered
    //!
    //! @return                     "true", if id is not used already, otherwise "false"
    //-----------------------------------------------------------------------------
    virtual bool add(SensorMapInterface::ID const &id) = 0;

    //-----------------------------------------------------------------------------
    //! Method which adds the given sensor data object (instance of SensorItemInterface)
    //! to the map under an automatically generated id.
    //!
    //! @param[in]      item        the sensor data object to be registered in this map
    //!
    //! @return                     "true", if name is not used already, otherwise "false"
    //-----------------------------------------------------------------------------
    virtual bool add(SensorMapInterface::Item *const item) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which removes the sensor data object with the given id from the map.
    //! The object is deleted after removing. Nothing will be done if there is
    //! no such object.
    //!
    //! @param[in]      id          the id of the object to be removed
    //!
    //! @return                     "true", if id exists in the map, otherwise
    //!                             "false"
    //-----------------------------------------------------------------------------
    virtual bool remove(SensorMapInterface::ID const &id) = 0;

    //-----------------------------------------------------------------------------
    //! Method which removes the given sensor data object from the map.
    //! The object is deleted after removing. Nothing will be done if the object is
    //! not in the map.
    //!
    //! @param[in]      item        the sensor data object to be removed
    //!
    //! @return                     "true", if item exists in the map, otherwise
    //!                             "false"
    //-----------------------------------------------------------------------------
    virtual bool remove(SensorMapInterface::Item *const item) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns a pointer to the sensor data object specified by the
    //! given id. If the map does not contain an object with the given id,
    //! nullptr is returned.
    //!
    //! @param[in]      id          the id of the sensor data object to be returned
    //!
    //! @return                     the pointer to the sensor data object having the
    //!                             given id, nullptr if given id does not exist
    //-----------------------------------------------------------------------------
    virtual SensorMapInterface::Item *getItem(SensorMapInterface::ID const &id) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method wich changes the id of the given sensor data object to the one passed
    //! as second argument.
    //!
    //! @param[in]      item        the pointer to a sensor data object
    //! @param[in]      id          the new id to be used for it
    //-----------------------------------------------------------------------------
    virtual void setID(SensorMapInterface::Item *const item,
                       SensorMapInterface::ID const &id) = 0;

    //-----------------------------------------------------------------------------
    //! Method wich returns the id of the given sensor data object, if contained
    //! in the map. Otherwise an empty id is returned.
    //!
    //! @param[in]      item        the pointer to the sensor data object in question
    //! @return                     the id of the object. Empty id if object is not
    //!                             in the map
    //-----------------------------------------------------------------------------
    virtual SensorMapInterface::ID getID(SensorMapInterface::Item *const item) const = 0;

    //-----------------------------------------------------------------------------
    //! Method wich generates an unoccupied id for this map. The generated id will be
    //! by one larger than the maximum of all occupied ids.
    //!
    //! @return                     an unoccupied id for this map
    //-----------------------------------------------------------------------------
    virtual SensorMapInterface::ID generateID() = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method wich checks whether the map contains a sensor data object of the given
    //! id.
    //!
    //! @param[in]      id        the id to be searched for
    //! @return                   true, if sensor data object with given id exists,
    //!                           otherwise false.
    //-----------------------------------------------------------------------------
    virtual bool contains(SensorMapInterface::ID const &id) const = 0;

    //-----------------------------------------------------------------------------
    //! Method which returns the number of items in the map.
    //!
    //! @return                     the number of items in the map
    //-----------------------------------------------------------------------------
    virtual int count() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which deletes all sensor data objects and clears the map.
    //-----------------------------------------------------------------------------
    virtual void clear() = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the list of all ids used in the map.
    //!
    //! @return                     the list of all ids used in the map
    //-----------------------------------------------------------------------------
    virtual QList<ID> keys() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which returns the list of all sensor data objects registered in this map.
    //!
    //! @return                     the list of all sensor data objects registered
    //!                             in this map.
    //-----------------------------------------------------------------------------
    virtual QList<Item *> values() const = 0;
};

#endif // SENSORMAPINTERFACE_H
