/****************************************************************************** 
* Copyright (c) 2017 Volkswagen Group of America. 
* 
* This program and the accompanying materials are made 
* available under the terms of the Eclipse Public License 2.0 
* which is available at https://www.eclipse.org/legal/epl-2.0/ 
* 
* SPDX-License-Identifier: EPL-2.0 
******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  ComponentInputMapInterface.h
//! @ingroup componentPlugin
//! @brief The Component Input Map Interface (CIMI) contains the functions
//! managing the registration and deregistration of the inputs of a component.
//! This interface belongs to the Component Plugin (CP).
//!
//-----------------------------------------------------------------------------

#ifndef COMPONENTINPUTMAPINTERFACE_H
#define COMPONENTINPUTMAPINTERFACE_H

#include <QMap>
#include <QObject>

#include "openPASS-Component/ComponentInputItemInterface.h"

/**
 * @brief This class manages the registration and deregistration of inputs
 * of a component. It provides a map as container with unique identifiers as
 * a keys to sort the inputs.
 */
class ComponentInputMapInterface : public QObject
{
    Q_OBJECT

public:
    using ID = ComponentInputItemInterface::ID; //!< The input's key
    using Item = ComponentInputItemInterface;   //!< The object defining input
    using Index = int;

public:
    using Map = QMap<ID, Item *>; //!< The container registering inputs
    using Iterator = Map::Iterator;
    using ConstIterator = Map::ConstIterator;

public:
    explicit ComponentInputMapInterface(QObject *const parent = nullptr) :
        QObject(parent)
    {
    }
    virtual ~ComponentInputMapInterface() = default;

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that a component input has been registered.
    //-----------------------------------------------------------------------------
    void added(Item *item) const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that all inputs have been successfully removed
    //! from the container (map).
    //-----------------------------------------------------------------------------
    void cleared() const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that a key id of a component input has been
    //! modified.
    //-----------------------------------------------------------------------------
    void modifiedID() const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that a component input has been deregistered.
    //-----------------------------------------------------------------------------
    void removed(Item *item) const;

public:
    //-----------------------------------------------------------------------------
    //! Registers a new input related-object under the key identifier \a id.
    //!
    //! @param[in]      id          The key id of the new input.
    //! @return                     True if input successfully registered.
    //-----------------------------------------------------------------------------
    virtual bool add(ComponentInputMapInterface::ID const &id) = 0;

    //-----------------------------------------------------------------------------
    //! Registers a given input related-object \a item with the key identifier
    //! \a id.
    //!
    //! @param[in]      id          The key id of the new input.
    //! @param[in]      item        The object defining the input.
    //! @return                     True if input successfully registered.
    //-----------------------------------------------------------------------------
    virtual bool add(ComponentInputMapInterface::ID const &id,
                     ComponentInputMapInterface::Item *const item) = 0;

    //-----------------------------------------------------------------------------
    //! Registers a given input related-object \a item with an automatically generated
    //! id.
    //!
    //! @param[in]      item        The object defining the input.
    //! @return                     True if input successfully registered.
    //-----------------------------------------------------------------------------
    virtual bool add(ComponentInputMapInterface::Item *const item) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing at the first component input related-object in
    //! the map.
    //!
    //! @return                     The iterator pointing at the first input in the
    //!                             map.
    //-----------------------------------------------------------------------------
    virtual ComponentInputMapInterface::Iterator begin() = 0;

    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing at the first component input related-object in
    //! the map (use for read-only since pointing at constant values).
    //!
    //! @return                     The iterator pointing at the first input in the
    //!                             map.
    //-----------------------------------------------------------------------------
    virtual ComponentInputMapInterface::ConstIterator begin() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Removes all the input related-objects registered (clear the container).
    //!
    //! @return                     True if all inputs successfully removed.
    //-----------------------------------------------------------------------------
    virtual bool clear() = 0;

public:
    //-----------------------------------------------------------------------------
    //! Counts the number of component input related-objects registered.
    //!
    //! @return                     The number of inputs registered.
    //-----------------------------------------------------------------------------
    virtual int count() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Verifies if the container (map) contains the component input related-object
    //! under the key identifier \a id.
    //!
    //! @param[in]      id          The key id of the new input.
    //! @return                     True if input already registered.
    //-----------------------------------------------------------------------------
    virtual bool contains(ComponentInputMapInterface::ID const &id) const = 0;

    //-----------------------------------------------------------------------------
    //! Verifies if the container (map) contains the component input related-object
    //! \a item.
    //!
    //! @param[in]      item        The object defining the input.
    //! @return                     True if input already registered.
    //-----------------------------------------------------------------------------
    virtual bool contains(ComponentInputMapInterface::Item *const item) const = 0;

    //-----------------------------------------------------------------------------
    //! Verifies if there is a component input related-object at the index position
    //! \a index in the container (map).
    //!
    //! @param[in]      index       The index position in the map.
    //! @return                     True if input already registered.
    //-----------------------------------------------------------------------------
    virtual bool contains(ComponentInputMapInterface::Index const &index) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing at the end of the map after the last component
    //! input related-object.
    //!
    //! @return                     The iterator pointing at the end of the map.
    //-----------------------------------------------------------------------------
    virtual ComponentInputMapInterface::Iterator end() = 0;

    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing at the end of the map after the last component
    //! input related-object (use for read-only).
    //!
    //! @return                     The iterator pointing at the end of the map.
    //-----------------------------------------------------------------------------
    virtual ComponentInputMapInterface::ConstIterator end() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets a key identifier \a id for the component input related-object \a item
    //! registered in the container (map).
    //!
    //! @param[in]      item        The object defining the input.
    //! @param[in]      id          The key id of the input.
    //! @return                     True if the key identifier successfully modified.
    //-----------------------------------------------------------------------------
    virtual bool setID(ComponentInputMapInterface::Item *const item,
                       ComponentInputMapInterface::ID const &id) = 0;

    //-----------------------------------------------------------------------------
    //! Generates a unique key identifier for the new input registered in the map.
    //!
    //! @return                     The key identifier of the input.
    //-----------------------------------------------------------------------------
    virtual ComponentInputMapInterface::ID generateID() const = 0;

    //-----------------------------------------------------------------------------
    //! Gets the key identifier of the component input related-object \a item from
    //! the container (map).
    //!
    //! @param[in]      item        The object defining the input.
    //! @return                     The key identifier of the input.
    //-----------------------------------------------------------------------------
    virtual ComponentInputMapInterface::ID getID(ComponentInputMapInterface::Item *const item) const = 0;

    //-----------------------------------------------------------------------------
    //! Gets the key identifier of the component input related-object in the index
    //! position \a index of the container (map).
    //!
    //! @param[in]      index       The index position in the map.
    //! @return                     The key identifier of the input.
    //-----------------------------------------------------------------------------
    virtual ComponentInputMapInterface::ID getID(ComponentInputMapInterface::Index const &index) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Gets the index position in the container (map) of the input related-object
    //! with the key identifier \a id.
    //!
    //! @param[in]      id          The key id of the input.
    //! @return                     The index position of the input in the map.
    //-----------------------------------------------------------------------------
    virtual ComponentInputMapInterface::Index getIndex(ComponentInputMapInterface::ID const &id) const = 0;

    //-----------------------------------------------------------------------------
    //! Gets the index position of the component input related-object \a item in
    //! the container (map).
    //!
    //! @param[in]      item        The object defining the input.
    //! @return                     The index position of the input in the map.
    //-----------------------------------------------------------------------------
    virtual ComponentInputMapInterface::Index getIndex(ComponentInputMapInterface::Item *const item) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Gets the input related-object under the key identifier \a id.
    //!
    //! @param[in]      id          The key identifier of the input.
    //! @return                     The object defining the input.
    //-----------------------------------------------------------------------------
    virtual ComponentInputMapInterface::Item *getItem(ComponentInputMapInterface::ID const &id) const = 0;

    //-----------------------------------------------------------------------------
    //! Gets the input related-object at the index position \a index in the map.
    //!
    //! @param[in]      index       The index position in the map.
    //! @return                     The object defining the input.
    //-----------------------------------------------------------------------------
    virtual ComponentInputMapInterface::Item *getItem(ComponentInputMapInterface::Index const &index) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Deregisters a input related-object under the key identifier \a id.
    //!
    //! @param[in]      id          The key identifier of the input.
    //! @return                     True if input successfully deregistered.
    //-----------------------------------------------------------------------------
    virtual bool remove(ComponentInputMapInterface::ID const &id) = 0;

    //-----------------------------------------------------------------------------
    //! Deregisters the input related-object \a item.
    //!
    //! @param[in]      item        The object defining the input.
    //! @return                     True if input successfully deregistered.
    //-----------------------------------------------------------------------------
    virtual bool remove(ComponentInputMapInterface::Item *const item) = 0;

    //-----------------------------------------------------------------------------
    //! Deregisters the input related-object at the index position \a index in the
    //! container (map).
    //!
    //! @param[in]      index       The index position in the map.
    //! @return                     True if input successfully deregistered.
    //-----------------------------------------------------------------------------
    virtual bool remove(ComponentInputMapInterface::Index const &index) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Creates a new component input item and returns a pointer to it.
    //! @note The item is not registered in this map!
    //!
    //! @return         a pointer to the newly created input item
    //-----------------------------------------------------------------------------
    virtual Item *createInput() const = 0;
};

#endif // COMPONENTINPUTMAPINTERFACE_H
