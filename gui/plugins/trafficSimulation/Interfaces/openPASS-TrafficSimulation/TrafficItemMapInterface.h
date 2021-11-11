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
//! @file  TrafficItemMapInterface.h
//! @ingroup trafficSimulationPlugin
//! @brief The Traffic Item Map Interface class defines the map in
//!        which traffic items of different kinds can be registered under a key
//!        identifier (ID). The traffic items are defined in the TrafficItemInterface
//!        class.
//-----------------------------------------------------------------------------
#ifndef TRAFFICITEMMAPINTERFACE_H
#define TRAFFICITEMMAPINTERFACE_H

#include <QList>
#include <QMap>

#include "TrafficItemInterface.h"

//-----------------------------------------------------------------------------
//! @brief The Traffic Item Map Interface class defines the map in
//!        which traffic items of different kinds can be registered under a key
//!        identifier (ID). The traffic items are defined in the TrafficItemInterface
//!        template class.
//-----------------------------------------------------------------------------
class TrafficItemMapInterface : public QObject
{
    Q_OBJECT

public:
    using ID = unsigned int;           //!< The key identifier (ID)
    using Item = TrafficItemInterface; //!< The traffic items registered in the map

public:
    using Map = QMap<ID, Item *>;             //!< The map for registering traffic items (using Qt's QMap class)
    using Iterator = Map::Iterator;           //!< An iterator for searching within the map
    using ConstIterator = Map::ConstIterator; //!< A constant iterator (for reading only)

public:
    explicit TrafficItemMapInterface(QObject *parent = nullptr) :
        QObject(parent)
    {
    }
    virtual ~TrafficItemMapInterface() = default;

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal emitted when traffic items are added to or removed from this map.
    //! It is also connected to the TrafficItemInterface::edited signal emitted
    //! when the value or probability of a traffic item has been edited.
    //-----------------------------------------------------------------------------
    void edited();

public:
    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing at the first traffic item in the map.
    //!
    //! @return                     The iterator pointing at the first object
    //!                             in the map.
    //-----------------------------------------------------------------------------
    virtual TrafficItemMapInterface::Iterator begin() = 0;

    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing at the first traffic item in the map
    //! (use for read-only since pointing at constant values).
    //!
    //! @return                     The iterator pointing at the first object
    //!                             in the map.
    //-----------------------------------------------------------------------------
    virtual TrafficItemMapInterface::ConstIterator constBegin() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing at the end of the map after the last traffic
    //! item.
    //!
    //! @return                     The iterator pointing at the end of the map.
    //-----------------------------------------------------------------------------
    virtual TrafficItemMapInterface::Iterator end() = 0;

    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing at the end of the map after the last traffic
    //! item (use for read-only).
    //!
    //! @return                     The iterator pointing at the end of the map.
    //-----------------------------------------------------------------------------
    virtual TrafficItemMapInterface::ConstIterator constEnd() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which creates a traffic item and registers
    //! it under the ID passed as a first argument, if not already used in the map.
    //! The value of the traffic item (datatype T) will be set to the one passed
    //! as a second argument. The probability is set to the default value of 0.5.
    //!
    //! @param[in]      id          the ID under which the newly created item is registered
    //! @param[in]      probability the probability of the traffic item
    //! @param[in]      value       the value of the traffic item
    //!
    //! @return                     "true", if given ID is not already used, otherwise "false"
    //-----------------------------------------------------------------------------
    virtual bool add(TrafficItemMapInterface::ID const &id,
                     TrafficItemInterface::Probability const &probability,
                     TrafficItemInterface::Value const &value) = 0;

    //-----------------------------------------------------------------------------
    //! Method which adds the given traffic item to the map,
    //! if not already contained in the map. The corresponding ID is generated using generateID().
    //!
    //! @param[in]      item         the traffic item to be registered
    //!
    //! @return                      "true" if item is not already contained in the map,
    //!                              otherwise "false"
    //-----------------------------------------------------------------------------
    virtual bool add(TrafficItemMapInterface::Item *const item) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which removes the traffic item registered under the given ID from the map.
    //! Afterwards, the item is deleted. Nothing will be done, if there is no item with
    //! the given ID is contained in the map.
    //!
    //! @param[in]      id           the ID whose corresponding item should be removed
    //!
    //! @return                      "true" if there is an item with the given ID, otherwise
    //!                              "false"
    //-----------------------------------------------------------------------------
    virtual bool remove(TrafficItemMapInterface::ID const &id) = 0;

    //-----------------------------------------------------------------------------
    //! Method which removes the given traffic item from the map.
    //! Afterwards, the item is deleted. Nothing will be done, if there is there no
    //! such item registered in the map.
    //!
    //! @param[in]      item         the traffic item which should be removed
    //!
    //! @return                      "true" if the item is contained in the map, otherwise
    //!                              "false"
    //-----------------------------------------------------------------------------
    virtual bool remove(TrafficItemMapInterface::Item *const item) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the traffic item registered
    //! under the given ID. If there is no such item in the map, nullptr is returned.
    //!
    //! @param[in]      id           the ID of the traffic item which should be returned
    //!
    //! @return                      the traffic item registered under the given ID if
    //!                              contained in the map, nullptr otherwise
    //-----------------------------------------------------------------------------
    virtual TrafficItemMapInterface::Item *getItem(TrafficItemMapInterface::ID const &id) const = 0;

    //-----------------------------------------------------------------------------
    //! Method which returns the ID of the given traffic item, if contained in the map.
    //! Returns Qt's default-constructed key if no such item is registered.
    //!
    //! @param[in]      item         the ID of the traffic item which should be returned
    //!
    //! @return                      the ID of the given traffic item if contained. Otherwise,
    //!                              default-constructed key
    //-----------------------------------------------------------------------------
    virtual TrafficItemMapInterface::ID getID(TrafficItemMapInterface::Item *const item) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the highest unoccupied ID of the map.
    //!
    //! @return                      the highest unocccupied ID of the map
    //-----------------------------------------------------------------------------
    virtual TrafficItemMapInterface::ID generateID() const = 0;

    //-----------------------------------------------------------------------------
    //! Method tests whether the map contains an entry under the given ID.
    //!
    //! @return                      "true", if there is an item under the given ID,
    //!                              "false" otherwise
    //-----------------------------------------------------------------------------
    virtual bool contains(TrafficItemMapInterface::ID const &id) const = 0;

    //-----------------------------------------------------------------------------
    //! Method which returns the number of items registered in the map.
    //!
    //! @return                      the number of items in the map
    //-----------------------------------------------------------------------------
    virtual int count() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which clears the map and deletes all items.
    //-----------------------------------------------------------------------------
    virtual void clear() = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns a list of all IDs used in the map. For the list, Qt's
    //! QList class is used.
    //!
    //! @return                     the list (QList) of all IDs used in the map
    //-----------------------------------------------------------------------------
    virtual QList<ID> keys() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which returns a list of all items registered in the map. For the list, Qt's
    //! QList class is used.
    //!
    //! @return                     the list (QList) of all items in the map
    //-----------------------------------------------------------------------------
    virtual QList<Item *> values() const = 0;
};

#endif // TRAFFICITEMMAPINTERFACE_H
