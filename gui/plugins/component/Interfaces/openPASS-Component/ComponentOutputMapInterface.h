/********************************************************************************
 * Copyright (c) 2017 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  ComponentOutputMapInterface.h
//! @ingroup componentPlugin
//! @brief The Component Output Map Interface (COMI) contains the functions
//! managing the registration and deregistration of the outputs of a component.
//! This interface belongs to the Component Plugin (CP).
//!
//-----------------------------------------------------------------------------

#ifndef COMPONENTOUTPUTMAPINTERFACE_H
#define COMPONENTOUTPUTMAPINTERFACE_H

#include <QMap>
#include <QObject>

#include "openPASS-Component/ComponentOutputItemInterface.h"

/**
 * @brief This class manages the registration and deregistration of outputs
 * of a component. It provides a map as container with unique identifiers as
 * a keys to sort the outputs.
 */
class ComponentOutputMapInterface : public QObject
{
    Q_OBJECT

public:
    using ID = ComponentOutputItemInterface::ID; //!< The output's key
    using Item = ComponentOutputItemInterface;   //!< The object defining output
    using Index = int;

public:
    using Map = QMap<ID, Item *>; //!< The container registering outputs
    using Iterator = Map::Iterator;
    using ConstIterator = Map::ConstIterator;

public:
    explicit ComponentOutputMapInterface(QObject *const parent = nullptr) :
        QObject(parent)
    {
    }
    virtual ~ComponentOutputMapInterface() = default;

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that a component output has been registered.
    //-----------------------------------------------------------------------------
    void added(Item *item) const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that all outputs have been successfully removed
    //! from the container (map).
    //-----------------------------------------------------------------------------
    void cleared() const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that a key id of a component output has been
    //! modified.
    //-----------------------------------------------------------------------------
    void modifiedID() const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that a component output has been deregistered.
    //-----------------------------------------------------------------------------
    void removed(Item *item) const;

public:
    //-----------------------------------------------------------------------------
    //! Registers a new output related-object under the key identifier \a id.
    //!
    //! @param[in]      id          The key id of the new output.
    //! @return                     True if output successfully registered.
    //-----------------------------------------------------------------------------
    virtual bool add(ComponentOutputMapInterface::ID const &id) = 0;

    //-----------------------------------------------------------------------------
    //! Registers a given output related-object \a item with the key identifier
    //! \a id.
    //!
    //! @param[in]      id          The key id of the new output.
    //! @param[in]      item        The object defining the output.
    //! @return                     True if output successfully registered.
    //-----------------------------------------------------------------------------
    virtual bool add(ComponentOutputMapInterface::ID const &id,
                     ComponentOutputMapInterface::Item *const item) = 0;

    //-----------------------------------------------------------------------------
    //! Registers a given output related-object \a item with an automatically generated
    //! id.
    //!
    //! @param[in]      item        The object defining the output.
    //! @return                     True if input successfully registered.
    //-----------------------------------------------------------------------------
    virtual bool add(ComponentOutputMapInterface::Item *const item) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing at the first component output related-object in
    //! the map.
    //!
    //! @return                     The iterator pointing at the first output in the
    //!                             map.
    //-----------------------------------------------------------------------------
    virtual ComponentOutputMapInterface::Iterator begin() = 0;

    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing at the first component output related-object in
    //! the map (use for read-only since pointing at constant values).
    //!
    //! @return                     The iterator pointing at the first output in the
    //!                             map.
    //-----------------------------------------------------------------------------
    virtual ComponentOutputMapInterface::ConstIterator begin() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Removes all the output related-objects registered (clear the container).
    //!
    //! @return                     True if all outputs successfully removed.
    //-----------------------------------------------------------------------------
    virtual bool clear() = 0;

public:
    //-----------------------------------------------------------------------------
    //! Counts the number of component output related-objects registered.
    //!
    //! @return                     The number of outputs registered.
    //-----------------------------------------------------------------------------
    virtual int count() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Verifies if the container (map) contains the component output related-object
    //! under the key identifier \a id.
    //!
    //! @param[in]      id          The key id of the new output.
    //! @return                     True if output already registered.
    //-----------------------------------------------------------------------------
    virtual bool contains(ComponentOutputMapInterface::ID const &id) const = 0;

    //-----------------------------------------------------------------------------
    //! Verifies if the container (map) contains the component output related-object
    //! \a item.
    //!
    //! @param[in]      item        The object defining the output.
    //! @return                     True if output already registered.
    //-----------------------------------------------------------------------------
    virtual bool contains(ComponentOutputMapInterface::Item *const item) const = 0;

    //-----------------------------------------------------------------------------
    //! Verifies if there is a component output related-object at the index position
    //! \a index in the container (map).
    //!
    //! @param[in]      index       The index position in the map.
    //! @return                     True if output already registered.
    //-----------------------------------------------------------------------------
    virtual bool contains(ComponentOutputMapInterface::Index const &index) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing at the end of the map after the last component
    //! output related-object.
    //!
    //! @return                     The iterator pointing at the end of the map.
    //-----------------------------------------------------------------------------
    virtual ComponentOutputMapInterface::Iterator end() = 0;

    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing at the end of the map after the last component
    //! output related-object (use for read-only).
    //!
    //! @return                     The iterator pointing at the end of the map.
    //-----------------------------------------------------------------------------
    virtual ComponentOutputMapInterface::ConstIterator end() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets a key identifier \a id for the component output related-object \a item
    //! registered in the container (map).
    //!
    //! @param[in]      item        The object defining the output.
    //! @param[in]      id          The key id of the output.
    //! @return                     True if the key identifier successfully modified.
    //-----------------------------------------------------------------------------
    virtual bool setID(ComponentOutputMapInterface::Item *const item,
                       ComponentOutputMapInterface::ID const &id) = 0;

    //-----------------------------------------------------------------------------
    //! Generates a unique key identifier for the new output registered in the map.
    //!
    //! @return                     The key identifier of the output.
    //-----------------------------------------------------------------------------
    virtual ComponentOutputMapInterface::ID generateID() const = 0;

    //-----------------------------------------------------------------------------
    //! Gets the key identifier of the component output related-object \a item from
    //! the container (map).
    //!
    //! @param[in]      item        The object defining the output.
    //! @return                     The key identifier of the output.
    //-----------------------------------------------------------------------------
    virtual ComponentOutputMapInterface::ID getID(ComponentOutputMapInterface::Item *const item) const = 0;

    //-----------------------------------------------------------------------------
    //! Gets the key identifier of the component output related-object in the index
    //! position \a index of the container (map).
    //!
    //! @param[in]      index       The index position in the map.
    //! @return                     The key identifier of the output.
    //-----------------------------------------------------------------------------
    virtual ComponentOutputMapInterface::ID getID(ComponentOutputMapInterface::Index const &index) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Gets the index position in the container (map) of the output related-object
    //! with the key identifier \a id.
    //!
    //! @param[in]      id          The key id of the output.
    //! @return                     The index position of the output in the map.
    //-----------------------------------------------------------------------------
    virtual ComponentOutputMapInterface::Index getIndex(ComponentOutputMapInterface::ID const &id) const = 0;

    //-----------------------------------------------------------------------------
    //! Gets the index position of the component output related-object \a item in
    //! the container (map).
    //!
    //! @param[in]      item        The object defining the output.
    //! @return                     The index position of the output in the map.
    //-----------------------------------------------------------------------------
    virtual ComponentOutputMapInterface::Index getIndex(ComponentOutputMapInterface::Item *const item) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Gets the output related-object under the key identifier \a id.
    //!
    //! @param[in]      id          The key identifier of the output.
    //! @return                     The object defining the output.
    //-----------------------------------------------------------------------------
    virtual ComponentOutputMapInterface::Item *getItem(ComponentOutputMapInterface::ID const &id) const = 0;

    //-----------------------------------------------------------------------------
    //! Gets the output related-object at the index position \a index in the map.
    //!
    //! @param[in]      index       The index position in the map.
    //! @return                     The object defining the output.
    //-----------------------------------------------------------------------------
    virtual ComponentOutputMapInterface::Item *getItem(ComponentOutputMapInterface::Index const &index) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Deregisters a output related-object under the key identifier \a id.
    //!
    //! @param[in]      id          The key identifier of the output.
    //! @return                     True if output successfully deregistered.
    //-----------------------------------------------------------------------------
    virtual bool remove(ComponentOutputMapInterface::ID const &id) = 0;

    //-----------------------------------------------------------------------------
    //! Deregisters the output related-object \a item.
    //!
    //! @param[in]      item        The object defining the output.
    //! @return                     True if output successfully deregistered.
    //-----------------------------------------------------------------------------
    virtual bool remove(ComponentOutputMapInterface::Item *const item) = 0;

    //-----------------------------------------------------------------------------
    //! Deregisters the output related-object at the index position \a index in the
    //! container (map).
    //!
    //! @param[in]      index       The index position in the map.
    //! @return                     True if output successfully deregistered.
    //-----------------------------------------------------------------------------
    virtual bool remove(ComponentOutputMapInterface::Index const &index) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Creates a component output item and returns a pointer to it.
    //!
    //! @note The item is not registered in this map!
    //!
    //! @return         a pointer to the newly created output item
    //-----------------------------------------------------------------------------
    virtual Item *createOutput() const = 0;
};

#endif // COMPONENTOUTPUTMAPINTERFACE_H
