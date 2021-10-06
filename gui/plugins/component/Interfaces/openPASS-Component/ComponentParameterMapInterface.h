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
//! @file  ComponentParameterMapInterface.h
//! @ingroup componentPlugin
//! @brief The Component Parameter Map Interface (CPMI) contains the functions
//! managing the registration and deregistration of the parameters of a component.
//! This interface belongs to the Component Plugin (CP).
//!
//-----------------------------------------------------------------------------

#ifndef COMPONENTPARAMETERMAPINTERFACE_H
#define COMPONENTPARAMETERMAPINTERFACE_H

#include <QMap>
#include <QObject>

#include "openPASS-Component/ComponentParameterItemInterface.h"
#include "openPASS-Component/ComponentTypedefMapInterface.h"

/**
 * @brief This class manages the registration and deregistration of parameters
 * of a component. It provides a map as container with unique identifiers as
 * a keys to sort the parameters.
 */
class ComponentParameterMapInterface : public QObject
{
    Q_OBJECT

public:
    using ID = ComponentParameterItemInterface::ID; //!< The parameter's key
    using Item = ComponentParameterItemInterface;   //!< The object defining the parameter
    using Index = int;

public:
    using Map = QMap<ID, Item *>; //!< The container registering parameters
    using Iterator = Map::Iterator;
    using ConstIterator = Map::ConstIterator;

public:
    explicit ComponentParameterMapInterface(QObject *const parent = nullptr) :
        QObject(parent)
    {
    }
    virtual ~ComponentParameterMapInterface() = default;

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that a component parameter has been registered.
    //-----------------------------------------------------------------------------
    void added(Item *item) const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that all parameters have been successfully removed
    //! from the container (map).
    //-----------------------------------------------------------------------------
    void cleared() const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that a key id of a component parameter has been
    //! modified.
    //-----------------------------------------------------------------------------
    void modifiedID() const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that a component parameter has been deregistered.
    //-----------------------------------------------------------------------------
    void removed(Item *item) const;

public:
    //-----------------------------------------------------------------------------
    //! Registers the given parameter under an automatically generated key.
    //!
    //! @param[in]      item          The key id of the new parameter.
    //! @return                     True if parameter successfully registered.
    //-----------------------------------------------------------------------------
    virtual bool add(ComponentParameterMapInterface::Item *const item) = 0;

    //-----------------------------------------------------------------------------
    //! Registers a new paramter related-object under the key identifier \a id.
    //!
    //! @param[in]      id          The key id of the new parameter.
    //! @return                     True if parameter successfully registered.
    //-----------------------------------------------------------------------------
    virtual bool add(ComponentParameterMapInterface::ID const &id) = 0;

    //-----------------------------------------------------------------------------
    //! Registers a given parameter related-object \a item with the key identifier
    //! \a id.
    //!
    //! @param[in]      id          The key id of the new parameter.
    //! @param[in]      item        The object defining the parameter.
    //! @return                     True if parameter successfully registered.
    //-----------------------------------------------------------------------------
    virtual bool add(ComponentParameterMapInterface::ID const &id,
                     ComponentParameterMapInterface::Item *const item) = 0;

    //-----------------------------------------------------------------------------
    //! Creates a new parameter object and returns a pointer to it. The parameter
    //! is not added to this map.
    //!
    //! @return                     a pointer to the newly created parameter
    //-----------------------------------------------------------------------------
    virtual ComponentParameterMapInterface::Item *createParameter() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing at the first component parameter related-object
    //! in the map.
    //!
    //! @return                     The iterator pointing at the first parameter
    //!                             in the map.
    //-----------------------------------------------------------------------------
    virtual ComponentParameterMapInterface::Iterator begin() = 0;

    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing at the first component parameter related-object
    //! in the map (use for read-only since pointing at constant values).
    //!
    //! @return                     The iterator pointing at the first parameter
    //!                             in the map.
    //-----------------------------------------------------------------------------
    virtual ComponentParameterMapInterface::ConstIterator begin() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Removes all the paramter related-objects registered (clear the container).
    //!
    //! @return                     True if all parameters successfully removed.
    //-----------------------------------------------------------------------------
    virtual bool clear() = 0;

public:
    //-----------------------------------------------------------------------------
    //! Counts the number of component parameter related-objects registered.
    //!
    //! @return                     The number of parameters registered.
    //-----------------------------------------------------------------------------
    virtual int count() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Verifies if the container (map) contains the component parameter related-object
    //! under the key identifier \a id.
    //!
    //! @param[in]      id          The key id of the new parameter.
    //! @return                     True if parameter already registered.
    //-----------------------------------------------------------------------------
    virtual bool contains(ComponentParameterMapInterface::ID const &id) const = 0;

    //-----------------------------------------------------------------------------
    //! Verifies if the container (map) contains the component parameter related-object
    //! \a item.
    //!
    //! @param[in]      item        The object defining the parameter.
    //! @return                     True if parameter already registered.
    //-----------------------------------------------------------------------------
    virtual bool contains(ComponentParameterMapInterface::Item *const item) const = 0;

    //-----------------------------------------------------------------------------
    //! Verifies if there is a component parameter related-object at the index position
    //! \a index in the container (map).
    //!
    //! @param[in]      index       The index position in the map.
    //! @return                     True if parameter already registered.
    //-----------------------------------------------------------------------------
    virtual bool contains(ComponentParameterMapInterface::Index const &index) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing at the end of the map after the last component
    //! parameter related-object.
    //!
    //! @return                     The iterator pointing at the end of the map.
    //-----------------------------------------------------------------------------
    virtual ComponentParameterMapInterface::Iterator end() = 0;

    //-----------------------------------------------------------------------------
    //! Gets an iterator pointing at the end of the map after the last component
    //! parameter related-object (use for read-only).
    //!
    //! @return                     The iterator pointing at the end of the map.
    //-----------------------------------------------------------------------------
    virtual ComponentParameterMapInterface::ConstIterator end() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets a key identifier \a id for the component parameter related-object \a item
    //! registered in the container (map).
    //!
    //! @param[in]      item        The object defining the parameter.
    //! @param[in]      id          The key id of the parameter.
    //! @return                     True if the key identifier successfully modified.
    //-----------------------------------------------------------------------------
    virtual bool setID(ComponentParameterMapInterface::Item *const item,
                       ComponentParameterMapInterface::ID const &id) = 0;

    //-----------------------------------------------------------------------------
    //! Generates a unique key identifier for the new parameter registered in the
    //! container (map).
    //!
    //! @return                     The key identifier of the parameter.
    //-----------------------------------------------------------------------------
    virtual ComponentParameterMapInterface::ID generateID() const = 0;

    //-----------------------------------------------------------------------------
    //! Gets the key identifier of the component parameter related-object \a item
    //! from the container (map).
    //!
    //! @param[in]      item        The object defining the parameter.
    //! @return                     The key identifier of the parameter.
    //-----------------------------------------------------------------------------
    virtual ComponentParameterMapInterface::ID getID(ComponentParameterMapInterface::Item *const item) const = 0;

    //-----------------------------------------------------------------------------
    //! Gets the key identifier of the component parameter related-object in the
    //! index position \a index of the container (map).
    //!
    //! @param[in]      index       The index position in the map.
    //! @return                     The key identifier of the parameter.
    //-----------------------------------------------------------------------------
    virtual ComponentParameterMapInterface::ID getID(ComponentParameterMapInterface::Index const &index) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Gets the index position in the container (map) of the component parameter
    //! related-object with the key identifier \a id.
    //!
    //! @param[in]      id          The key id of the parameter.
    //! @return                     The index position of the parameter in the map.
    //-----------------------------------------------------------------------------
    virtual ComponentParameterMapInterface::Index getIndex(ComponentParameterMapInterface::ID const &id) const = 0;

    //-----------------------------------------------------------------------------
    //! Gets the index position of the component parameter related-object \a item
    //! in the container (map).
    //!
    //! @param[in]      item        The object defining the parameter.
    //! @return                     The index position of the parameter in the map.
    //-----------------------------------------------------------------------------
    virtual ComponentParameterMapInterface::Index getIndex(ComponentParameterMapInterface::Item *const item) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Gets the parameter related-object under the key identifier \a id.
    //!
    //! @param[in]      id          The key identifier of the parameter.
    //! @return                     The object defining the parameter.
    //-----------------------------------------------------------------------------
    virtual ComponentParameterMapInterface::Item *getItem(ComponentParameterMapInterface::ID const &id) const = 0;

    //-----------------------------------------------------------------------------
    //! Gets the parameter related-object at the index position \a index in the
    //! container (map).
    //!
    //! @param[in]      index       The index position in the map.
    //! @return                     The object defining the parameter.
    //-----------------------------------------------------------------------------
    virtual ComponentParameterMapInterface::Item *getItem(ComponentParameterMapInterface::Index const &index) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Deregisters a parameter related-object under the key identifier \a id.
    //!
    //! @param[in]      id          The key identifier of the parameter.
    //! @return                     True if parameter successfully deregistered.
    //-----------------------------------------------------------------------------
    virtual bool remove(ComponentParameterMapInterface::ID const &id) = 0;

    //-----------------------------------------------------------------------------
    //! Deregisters the parameter related-object \a item.
    //!
    //! @param[in]      item        The object defining the parameter.
    //! @return                     True if parameter successfully deregistered.
    //-----------------------------------------------------------------------------
    virtual bool remove(ComponentParameterMapInterface::Item *const item) = 0;

    //-----------------------------------------------------------------------------
    //! Deregisters the parameter related-object at the index position \a index in
    //! the container (map).
    //!
    //! @param[in]      index       The index position in the map.
    //! @return                     True if parameter successfully deregistered.
    //-----------------------------------------------------------------------------
    virtual bool remove(ComponentParameterMapInterface::Index const &index) = 0;

public:
    virtual ComponentTypedefMapInterface *getParameterTypeMap() const = 0;
};

#endif // COMPONENTOUTPUTMAPINTERFACE_H
