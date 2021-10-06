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
//! @file  ComponentItemInterface.h
//! @ingroup componentPlugin
//! @brief The Component Item Interface (CII) contains the functions setters
//! and getters of the items constituing a component.
//! This interface belongs to the Component Plugin (CP).
//!
//-----------------------------------------------------------------------------

#ifndef COMPONENTITEMINTERFACE_H
#define COMPONENTITEMINTERFACE_H

#include <QObject>
#include <QString>

#include "openPASS-Component/ComponentInputMapInterface.h"
#include "openPASS-Component/ComponentOutputMapInterface.h"
#include "openPASS-Component/ComponentParameterMapInterface.h"
#include "openPASS-Component/ComponentScheduleInterface.h"

/**
 * @brief This class defines the items constituing a component:
 *      - the key name (must be UNIQUE)
 *      - the type: Action or Algorithm or Sensor
 *      - the title (modifiable)
 *      - the schedule: Priority, Offset, Cycle Time, Response Time
 *      - the parameters
 *      - the inputs
 *      - the outputs
 */
class ComponentItemInterface : public QObject
{
    Q_OBJECT

public:
    using Name = QString;           //!< The component's key name
    enum class Type : unsigned char //!< The type of the component
    {
        Action = 0,
        Algorithm = 1,
        Sensor = 2,
        Misc = 3,
        Logical = 4,
        Subsystem = 5
    };
    using Title = QString;                               //!< The Title of the component
    using Schedule = ComponentScheduleInterface;         //!< The object registering the parameters related to simulation schedule
    using ParameterMap = ComponentParameterMapInterface; //!< The container registering the parameters of the component
    using InputMap = ComponentInputMapInterface;         //!< The container registering the inputs of the component
    using OutputMap = ComponentOutputMapInterface;       //!< The container registering the outputs of the component

public:
    explicit ComponentItemInterface(QObject *const parent = nullptr) :
        QObject(parent)
    {
    }
    virtual ~ComponentItemInterface() = default;

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that a component has been reset successfully
    //! (used to update the Window View of the Component Generator when users
    //! pressing on <New Component> button).
    //-----------------------------------------------------------------------------
    void cleared() const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that a component has been loaded successfully
    //! (e.g. used to update the Window View of the Component Generator when users
    //! pressing on <Load Component> button).
    //-----------------------------------------------------------------------------
    void loaded() const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that a component has been saved successfully.
    //-----------------------------------------------------------------------------
    void saved() const;

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that the type of a component has been modified.
    //-----------------------------------------------------------------------------
    void modifiedType() const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that the title of a component has been modified.
    //-----------------------------------------------------------------------------
    void modifiedTitle() const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that the name (library) of a component has been modified.
    //-----------------------------------------------------------------------------
    void modifiedName();

public:
    //-----------------------------------------------------------------------------
    //! Clears the items constituing the component.
    //! (e.g. function called when users pressing on <New Component> button in the
    //! Window View of the Component Generator).
    //!
    //! @return                     True if component items successfully cleared.
    //-----------------------------------------------------------------------------
    virtual bool clear() = 0;

    //-----------------------------------------------------------------------------
    //! Loads a component from a file located in the given path \a filepath.
    //! (e.g. function called when users pressing on <Load Component> button in the
    //! Window View of the Component Generator).
    //!
    //! @param[in]      filepath    The path locating the file defining the component.
    //! @return                     True if component successfully loaded.
    //-----------------------------------------------------------------------------
    virtual bool load(QString const &filepath) = 0;

    //-----------------------------------------------------------------------------
    //! Saves a component to a file stored in the given path \a filepath.
    //! (e.g. function called when users pressing on <Save Component> button in the
    //! Window View of the Component Generator).
    //!
    //! @param[in]      filepath    The path for storing the file defining the component.
    //! @return                     True if component successfully saved.
    //-----------------------------------------------------------------------------
    virtual bool save(QString const &filepath) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Gets the key name of the current component registered in the container (map).
    //!
    //! @return                     The key name of the component.
    //-----------------------------------------------------------------------------
    virtual ComponentItemInterface::Name getName() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets the type of the current registered component.
    //!
    //! @return                     True if the component type successfully modified.
    //-----------------------------------------------------------------------------
    virtual bool setType(ComponentItemInterface::Type const &type) = 0;

    //-----------------------------------------------------------------------------
    //! Gets the type of the current registered component.
    //!
    //! @return                     The type of the component.
    //-----------------------------------------------------------------------------
    virtual ComponentItemInterface::Type getType() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets a title to the current registered component.
    //!
    //! @return                     True if the title successfully modified.
    //-----------------------------------------------------------------------------
    virtual bool setTitle(ComponentItemInterface::Title const &title) = 0;

    //-----------------------------------------------------------------------------
    //! Gets the title of the current registered component.
    //!
    //! @return                     The title of the component.
    //-----------------------------------------------------------------------------
    virtual ComponentItemInterface::Title getTitle() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Gets the simulation schedule related parameters of the current registered
    //! component.
    //!
    //! @return                     The schedule of the component.
    //-----------------------------------------------------------------------------
    virtual ComponentItemInterface::Schedule *getSchedule() const = 0;

    //-----------------------------------------------------------------------------
    //! Gets the parameters of the current registered component.
    //!
    //! @return                     The parameters of the component.
    //-----------------------------------------------------------------------------
    virtual ComponentItemInterface::ParameterMap *getParameters() const = 0;

    //-----------------------------------------------------------------------------
    //! Gets the inputs of the current registered component.
    //!
    //! @return                     The inputs of the component.
    //-----------------------------------------------------------------------------
    virtual ComponentItemInterface::InputMap *getInputs() const = 0;

    //-----------------------------------------------------------------------------
    //! Gets the outputs of the current registered component.
    //!
    //! @return                     The outputs of the component.
    //-----------------------------------------------------------------------------
    virtual ComponentItemInterface::OutputMap *getOutputs() const = 0;
};

#endif // COMPONENTITEMINTERFACE_H
