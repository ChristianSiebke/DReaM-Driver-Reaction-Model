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
//! @file  ComponentInterface.h
//! @ingroup componentPlugin
//! @brief The Component Interface (CI) contains the functions creating components.
//! This interface belongs to the Component Plugin (CP).
//!
//-----------------------------------------------------------------------------

#ifndef COMPONENTINTERFACE_H
#define COMPONENTINTERFACE_H

#include "openPASS/ServiceInterface.h"
#include "openPASS-Component/ComponentItemInterface.h"
#include "openPASS-Component/ComponentMapInterface.h"
#include <QObject>

static ServiceManagerInterface::ID const ComponentInterfaceID =
        ServiceManagerInterfaceID("openPASS.Component.ComponentInterface");

/**
 * @brief This class manages creating new component and also creating the map
 * which registers created components.
 */
class ComponentInterface : public QObject,
        public ServiceInterface<ComponentInterface, ComponentInterfaceID>
{
    Q_OBJECT

public:
    using Item = ComponentItemInterface;    //!< The component
    using Map = ComponentMapInterface;      //!< The map for registering components

public:
    explicit ComponentInterface(QObject * const parent = nullptr)
        : QObject(parent), ServiceInterface(this) {}
    virtual ~ComponentInterface() = default;

public:
    //-----------------------------------------------------------------------------
    //! Creates the object defining a component with as parent object \a parent.
    //!
    //! @param[in]      parent      The object parent (e.g. Component Generator).
    //! @return                     The new component created.
    //-----------------------------------------------------------------------------
    virtual ComponentInterface::Item * createComponentItem(QObject * const parent) const = 0;

    //-----------------------------------------------------------------------------
    //! Creates a map to register components which is owned by the parent object
    //! \a parent.
    //!
    //! @param[in]      parent      The object parent (e.g. Manager of components library).
    //! @return                     The map registering components.
    //-----------------------------------------------------------------------------
    virtual ComponentInterface::Map * createComponentMap(QObject * const parent) const = 0;
};

#endif // COMPONENTINTERFACE_H
