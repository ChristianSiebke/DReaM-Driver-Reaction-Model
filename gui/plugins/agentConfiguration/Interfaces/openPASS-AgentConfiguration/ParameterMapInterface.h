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
//! @file  ParameterMapInterface.h
//! @ingroup agentConfigurationPlugin
//! @brief  This class constitutes a container class for parameters
//!         (instances of ParameterItemInterface). It uses the
//!         functionality of Qt's QMap class but inherits from QObject class
//!         in order to utilize Qt's parenting system. Items registered in this
//!         map always become child of this map.
//-----------------------------------------------------------------------------

#ifndef PARAMETERMAPINTERFACE_H
#define PARAMETERMAPINTERFACE_H

#include <QMap>
#include <QObject>

#include "openPASS-AgentConfiguration/ParameterItemInterface.h"
//-----------------------------------------------------------------------------
//! @brief  This class constitutes a container class for parameters
//!         (instances of ParameterItemInterface). It uses the
//!         functionality of Qt's QMap class but inherits from QObject class
//!         in order to utilize Qt's parenting system. Items registered in this
//!         map always become child of this map.
//-----------------------------------------------------------------------------
class ParameterMapInterface : public QObject
{
    Q_OBJECT

public:
    using ID = ParameterItemInterface::ID; //!< typedef for the id of a parameter
    using Item = ParameterItemInterface;   //!< typedef for the parameter

public:
    using Map = QMap<ID, Item *>;             //!< typedef for the map of parameters
    using Iterator = Map::Iterator;           //!< typedef for an iterator
    using ConstIterator = Map::ConstIterator; //!< typedef for a constant iterator (read-only operations)

public:
    ParameterMapInterface(QObject *const parent = nullptr) :
        QObject(parent)
    {
    }
    virtual ~ParameterMapInterface() = default;

public:
    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing to the first parameter in the map.
    //!
    //! @return                     The iterator pointing at the first object
    //!                             in the map.
    //-----------------------------------------------------------------------------
    virtual ParameterMapInterface::Iterator begin() = 0;

    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing to the first parameter in the map
    //! (use for read-only since pointing at constant values).
    //!
    //! @return                     The iterator pointing at the first object
    //!                             in the map.
    //-----------------------------------------------------------------------------
    virtual ParameterMapInterface::ConstIterator constBegin() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing to the end of the map after the last parameter.
    //!
    //! @return                     The iterator pointing at the end of the map.
    //-----------------------------------------------------------------------------
    virtual ParameterMapInterface::Iterator end() = 0;

    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing at the end of the map after the last parameter
    //! (use for read-only).
    //!
    //! @return                     The iterator pointing at the end of the map.
    //-----------------------------------------------------------------------------
    virtual ParameterMapInterface::ConstIterator constEnd() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which creates a parameter (instance of ParameterItemInterface) and
    //! adds it to the map under the given id.
    //!
    //! @param[in]      id        the id under which the newly created item is registered
    //!
    //! @return                   "true", if id is not used already, otherwise "false"
    //-----------------------------------------------------------------------------
    virtual bool add(ParameterMapInterface::ID const &id) = 0;

    //-----------------------------------------------------------------------------
    //! Method which creates a new parameter (instance of ParameterItemInterface) as a copy
    //! of the given parameter (second item) and registers it under the id passed as
    //! a first argument.
    //!
    //! @param[in]      id        the id to be used for the copied parameter
    //! @param[in]      item      the parameter to be copied
    //! @return                   "true", if id is not used already, otherwise "false"
    //-----------------------------------------------------------------------------
    virtual bool add(ParameterMapInterface::ID const &id,
                     ParameterMapInterface::Item const *const item) = 0;

    //-----------------------------------------------------------------------------
    //! Method which adds the given parameter (instance of ParameterItemInterface)
    //! to the map under an automatically generated id.
    //!
    //! @param[in]      item        the parameter to be registered in this map
    //!
    //! @return                     "true", if id is not used already, otherwise "false"
    //-----------------------------------------------------------------------------
    virtual bool add(ParameterMapInterface::Item *const item) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which removes the parameter with the given id from the map.
    //! The parameter is deleted after removing. Nothing will be done if there is
    //! no such parameter.
    //!
    //! @param[in]      id          the id of the object to be removed
    //!
    //! @return                     "true", if id exists in the map, otherwise
    //!                             "false"
    //-----------------------------------------------------------------------------
    virtual bool remove(ParameterMapInterface::ID const &id) = 0;

    //-----------------------------------------------------------------------------
    //! Method which removes the given parameter from the map. The parameter is
    //! deleted after removing. Nothing will be done if the parameter is not in the
    //! map.
    //!
    //! @param[in]      item        the parameter to be removed
    //!
    //! @return                     "true", if parameter exists in the map, otherwise
    //!                             "false"
    //-----------------------------------------------------------------------------
    virtual bool remove(ParameterMapInterface::Item *const item) = 0;

public:
    virtual ParameterMapInterface::Item *getItem(ParameterMapInterface::ID const &id) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method wich changes the id of the given parameter to the one passed
    //! as second argument.
    //!
    //! @param[in]      item        the pointer to a parameter
    //! @param[in]      id          the new id to be used for it
    //-----------------------------------------------------------------------------
    virtual void setID(ParameterMapInterface::Item *const item,
                       ParameterMapInterface::ID const &id) = 0;

    //-----------------------------------------------------------------------------
    //! Method wich returns the id of the given parameter, if contained
    //! in the map. Otherwise an empty id is returned.
    //!
    //! @param[in]      item        the pointer to the sensor data object in question
    //! @return                     the id of the object. Empty id if object is not
    //!                             in the map
    //-----------------------------------------------------------------------------
    virtual ParameterMapInterface::ID getID(ParameterMapInterface::Item *const item) const = 0;

    //-----------------------------------------------------------------------------
    //! Method wich generates an unoccupied id for this map. The generated id will be
    //! by one larger than the maximum of all occupied ids.
    //!
    //! @return                     an unoccupied id for this map
    //-----------------------------------------------------------------------------
    virtual ParameterMapInterface::ID generateID() = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method wich checks whether the map contains a parameter of the given
    //! id.
    //!
    //! @param[in]      id        the id to be searched for
    //! @return                   true, if a parameter with given id exists,
    //!                           otherwise false.
    //-----------------------------------------------------------------------------
    virtual bool contains(ParameterMapInterface::ID const &id) const = 0;

    //-----------------------------------------------------------------------------
    //! Method which returns the number of parameter in the map.
    //!
    //! @return                     the number of parameter in the map
    //-----------------------------------------------------------------------------
    virtual int count() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which deletes all registered parameters and clears the map.
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
    //! Method which returns the list of all parameters registered in this map.
    //!
    //! @return                     the list of all parameters registered
    //!                             in this map.
    //-----------------------------------------------------------------------------
    virtual QList<Item *> values() const = 0;
};

#endif //PARAMETERMAPINTERFACE_H
