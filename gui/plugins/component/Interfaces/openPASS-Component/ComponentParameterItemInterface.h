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
//! @file  ComponentParameterItemInterface.h
//! @ingroup componentPlugin
//! @brief The Component Parameter Item Interface (CPII) contains the functions
//! setters and getters of the items constituing a parameter of a component.
//! This interface belongs to the Component Plugin (CP).
//!
//-----------------------------------------------------------------------------

#ifndef COMPONENTPARAMETERITEMINTERFACE_H
#define COMPONENTPARAMETERITEMINTERFACE_H

#include <QObject>
#include <QString>

/**
 * @brief This class defines the items constituing a parameter of a component:
 *      - the key identifier (must be UNIQUE)
 *      - the type: Boolean or Integer or Float or Double or String
 *      - the title (modifiable)
 *      - the unit (should be SI unit)
 *      - the value
 */
class ComponentParameterItemInterface : public QObject
{
    Q_OBJECT

public:
    using ID = unsigned int;
    using Type = QString;
    using Title = QString;
    using Unit = QString;
    using Value = QString;

public:
    explicit ComponentParameterItemInterface(QObject *const parent = nullptr) :
        QObject(parent)
    {
    }
    virtual ~ComponentParameterItemInterface() = default;

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that a component parameter has been successfully
    //! removed (used to update the table of parameters in the Window View of the
    //! Component Generator when users pressing on Delete button).
    //-----------------------------------------------------------------------------
    void cleared() const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that the type of a component parameter has been
    //! modified.
    //-----------------------------------------------------------------------------
    void modifiedType() const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that the title of a component parameter has been
    //! modified.
    //-----------------------------------------------------------------------------
    void modifiedTitle() const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that the unit of a component parameter has been
    //! modified.
    //-----------------------------------------------------------------------------
    void modifiedUnit() const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that the value of a component parameter has been
    //! modified.
    //-----------------------------------------------------------------------------
    void modifiedValue() const;

public:
    //-----------------------------------------------------------------------------
    //! Resets the items constituing the parameter of the component.
    //! (e.g. function called when users pressing on Delete button of the table of
    //! parameters in the Window View of the Component Generator).
    //!
    //! @return                     True if parameter items successfully cleared.
    //-----------------------------------------------------------------------------
    virtual bool clear() = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets a key identifier \a id for the current registered parameter.
    //!
    //! @param[in]      id          The key identifier of the parameter.
    //! @return                     True if the parameter id successfully modified.
    //-----------------------------------------------------------------------------
    virtual bool setID(ComponentParameterItemInterface::ID const &id) = 0;

    //-----------------------------------------------------------------------------
    //! Gets the key identifier of the current registered parameter.
    //!
    //! @return                     The key identifier of the parameter.
    //-----------------------------------------------------------------------------
    virtual ComponentParameterItemInterface::ID getID() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets the type of the current registered parameter.
    //!
    //! @param[in]      type        The type of the parameter.
    //! @return                     True if the parameter type successfully modified.
    //-----------------------------------------------------------------------------
    virtual bool setType(ComponentParameterItemInterface::Type const &type) = 0;

    //-----------------------------------------------------------------------------
    //! Gets the type of the current registered parameter.
    //!
    //! @return                     The type of the parameter.
    //-----------------------------------------------------------------------------
    virtual ComponentParameterItemInterface::Type getType() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets the title of the current registered parameter.
    //!
    //! @param[in]      title       The title of the parameter.
    //! @return                     True if the parameter title successfully modified.
    //-----------------------------------------------------------------------------
    virtual bool setTitle(ComponentParameterItemInterface::Title const &title) = 0;

    //-----------------------------------------------------------------------------
    //! Gets the title of the current registered parameter.
    //!
    //! @return                     The title of the parameter.
    //-----------------------------------------------------------------------------
    virtual ComponentParameterItemInterface::Title getTitle() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets the unit of the current registered parameter.
    //!
    //! @param[in]      unit        The unit of the parameter.
    //! @return                     True if the parameter unit successfully modified.
    //-----------------------------------------------------------------------------
    virtual bool setUnit(ComponentParameterItemInterface::Unit const &unit) = 0;

    //-----------------------------------------------------------------------------
    //! Gets the unit of the current registered parameter.
    //!
    //! @return                     The unit of the parameter.
    //-----------------------------------------------------------------------------
    virtual ComponentParameterItemInterface::Unit getUnit() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets the value of the current registered parameter.
    //!
    //! @param[in]      value       The value of the parameter.
    //! @return                     True if the parameter value successfully modified.
    //-----------------------------------------------------------------------------
    virtual bool setValue(ComponentParameterItemInterface::Value const &value) = 0;

    //-----------------------------------------------------------------------------
    //! Gets the value of the current registered parameter.
    //!
    //! @return                     The value of the parameter.
    //-----------------------------------------------------------------------------
    virtual ComponentParameterItemInterface::Value getValue() const = 0;
};

#endif // COMPONENTPARAMETERITEMINTERFACE_H
