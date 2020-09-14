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
//! @file  ComponentMapInterface.h
//! @ingroup componentPlugin
//! @brief The Component Map Interface (CMI) contains the functions managing
//! the registration and deregistration of components. This interface belongs
//! to the Component Plugin (CP).
//!
//-----------------------------------------------------------------------------

#ifndef COMPONENTMAPINTERFACE_H
#define COMPONENTMAPINTERFACE_H

#include <QMap>
#include <QObject>
#include <QString>

#include "openPASS-Component/ComponentItemInterface.h"

/**
 * @brief This class manages the registration and deregistration of objects defining
 * a component (component related-objects) using as identifier a key name.
 */
class ComponentMapInterface : public QObject
{
    Q_OBJECT

public:
    using Name = ComponentItemInterface::Name; //!< The component's key name
    using Item = ComponentItemInterface;       //!< The object defining the component
    using Index = int;

public:
    using Map = QMap<Name, Item *>; //!< The container registering components
    using Iterator = Map::Iterator;
    using ConstIterator = Map::ConstIterator;

public:
    explicit ComponentMapInterface(QObject *const parent = nullptr) :
        QObject(parent)
    {
    }
    virtual ~ComponentMapInterface() = default;

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that a component has been registered.
    //-----------------------------------------------------------------------------
    void added() const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that a key name of a registered component has been
    //! modified.
    //-----------------------------------------------------------------------------
    void modifiedName() const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that a component has been deregistered.
    //-----------------------------------------------------------------------------
    void removed() const;

public:
    //-----------------------------------------------------------------------------
    //! Registers a new component related-object under the key name \a name.
    //!
    //! @param[in]      name        The key name of the new component.
    //! @return                     True if object successfully registered.
    //-----------------------------------------------------------------------------
    virtual bool add(ComponentMapInterface::Name const &name) = 0;

    //-----------------------------------------------------------------------------
    //! Registers a given component related-object \a item with the key name \a name.
    //!
    //! @param[in]      name        The key name of the new component.
    //! @param[in]      item        The object defining the component.
    //! @return                     True if object successfully registered.
    //-----------------------------------------------------------------------------
    virtual bool add(ComponentMapInterface::Name const &name,
                     ComponentMapInterface::Item *const item) = 0;

    //-----------------------------------------------------------------------------
    //! Registers a component related-object \a item with the key name \a name from
    //! a file located in the given path \a filepath (e.g. function called for
    //! loading a library of components).
    //!
    //! @param[in]      name        The key name of the new component.
    //! @param[in]      filepath    The path locating the file defining the component.
    //! @return                     True if object successfully registered.
    //-----------------------------------------------------------------------------
    virtual bool add(ComponentMapInterface::Name const &name,
                     QString const &filepath) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing at the first component related-object in the map.
    //!
    //! @return                     The iterator pointing at the first object
    //!                             in the map.
    //-----------------------------------------------------------------------------
    virtual ComponentMapInterface::Iterator begin() = 0;

    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing at the first component related-object in the map
    //! (use for read-only since pointing at constant values).
    //!
    //! @return                     The iterator pointing at the first object
    //!                             in the map.
    //-----------------------------------------------------------------------------
    virtual ComponentMapInterface::ConstIterator begin() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Removes all the component related-objects registered (clear the container).
    //!
    //! @return                     True if all objects successfully removed.
    //-----------------------------------------------------------------------------
    virtual bool clear() = 0;

public:
    //-----------------------------------------------------------------------------
    //! Counts the number of component related-objects registered.
    //!
    //! @return                     The number of objects registered.
    //-----------------------------------------------------------------------------
    virtual int count() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Verifies if the container (map) contains the component related-object under
    //! the key name \a name.
    //!
    //! @param[in]      name        The key name of the component.
    //! @return                     True if component already registered.
    //-----------------------------------------------------------------------------
    virtual bool contains(ComponentMapInterface::Name const &name) const = 0;

    //-----------------------------------------------------------------------------
    //! Verifies if the container (map) contains the component related-object \a item.
    //!
    //! @param[in]      item        The object defining the component.
    //! @return                     True if component already registered.
    //-----------------------------------------------------------------------------
    virtual bool contains(ComponentMapInterface::Item *const item) const = 0;

    //-----------------------------------------------------------------------------
    //! Verifies if there is a component related-object at the index position
    //! \a index in the container (map).
    //!
    //! @param[in]      index       The index position in the map.
    //! @return                     True if component already registered.
    //-----------------------------------------------------------------------------
    virtual bool contains(ComponentMapInterface::Index const &index) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing at the end of the map after the last component
    //! related-object.
    //!
    //! @return                     The iterator pointing at the end of the map.
    //-----------------------------------------------------------------------------
    virtual ComponentMapInterface::Iterator end() = 0;

    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing at the end of the map after the last component
    //! related-object (use for read-only).
    //!
    //! @return                     The iterator pointing at the end of the map.
    //-----------------------------------------------------------------------------
    virtual ComponentMapInterface::ConstIterator end() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets a key name \a name for the component related-object \a item registered
    //! in the container (map).
    //!
    //! @param[in]      item        The object defining the component.
    //! @param[in]      name        The key name of the component.
    //! @return                     True if the key name successfully modified.
    //-----------------------------------------------------------------------------
    virtual bool setName(ComponentMapInterface::Item *const item,
                         ComponentMapInterface::Name const &name) = 0;

    //-----------------------------------------------------------------------------
    //! Gets the key name of the component related-object \a item from the
    //! container (map).
    //!
    //! @param[in]      item        The object defining the component.
    //! @return                     The key name of the component.
    //-----------------------------------------------------------------------------
    virtual ComponentMapInterface::Name getName(ComponentMapInterface::Item *const item) const = 0;

    //-----------------------------------------------------------------------------
    //! Gets the key name of the component related-object in the index position
    //! \a index of the container (map).
    //!
    //! @param[in]      index       The index position in the map.
    //! @return                     The key name of the component.
    //-----------------------------------------------------------------------------
    virtual ComponentMapInterface::Name getName(ComponentMapInterface::Index const &index) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Gets the index position in the container (map) of the component related-object
    //! with the key name \a name.
    //!
    //! @param[in]      name        The key name of the component.
    //! @return                     The index position of the object in the map.
    //-----------------------------------------------------------------------------
    virtual ComponentMapInterface::Index getIndex(ComponentMapInterface::Name const &name) const = 0;

    //-----------------------------------------------------------------------------
    //! Gets the index position of the component related-object \a item in the
    //! container (map).
    //!
    //! @param[in]      item        The object defining the component.
    //! @return                     The index position of the object in the map.
    //-----------------------------------------------------------------------------
    virtual ComponentMapInterface::Index getIndex(ComponentMapInterface::Item *const item) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Gets the component related-object under the key name \a name.
    //!
    //! @param[in]      name        The key name of the component.
    //! @return                     The object defining the component.
    //-----------------------------------------------------------------------------
    virtual ComponentMapInterface::Item *getItem(ComponentMapInterface::Name const &name) const = 0;

    //-----------------------------------------------------------------------------
    //! Gets the component related-object at the index position \a index in the
    //! container (map).
    //!
    //! @param[in]      index       The index position in the map.
    //! @return                     The object defining the component.
    //-----------------------------------------------------------------------------
    virtual ComponentMapInterface::Item *getItem(ComponentMapInterface::Index const &index) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Deregisters a component related-object under the key name \a name.
    //!
    //! @param[in]      name        The key name of the component.
    //! @return                     True if object successfully deregistered.
    //-----------------------------------------------------------------------------
    virtual bool remove(ComponentMapInterface::Name const &name) = 0;

    //-----------------------------------------------------------------------------
    //! Deregisters the component related-object \a item.
    //!
    //! @param[in]      item        The object defining the component.
    //! @return                     True if object successfully deregistered.
    //-----------------------------------------------------------------------------
    virtual bool remove(ComponentMapInterface::Item *const item) = 0;

    //-----------------------------------------------------------------------------
    //! Deregisters the component related-object at the index position \a index in
    //! the container (map).
    //!
    //! @param[in]      index       The index position in the map.
    //! @return                     True if object successfully deregistered.
    //-----------------------------------------------------------------------------
    virtual bool remove(ComponentMapInterface::Index const &index) = 0;
    virtual bool remove(ComponentItemInterface::Type const &type) = 0;

public:
    virtual QList<Item *> values() const = 0;
};

#endif // COMPONENTMAPINTERFACE_H
