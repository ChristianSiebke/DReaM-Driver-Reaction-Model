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
//! @file  ComponentOutputItemInterface.h
//! @ingroup componentPlugin
//! @brief The Component Output Item Interface (COII) contains the functions
//! setters and getters of the items constituing an output of a component.
//! This interface belongs to the Component Plugin (CP).
//!
//-----------------------------------------------------------------------------

#ifndef COMPONENTOUTPUTITEMINTERFACE_H
#define COMPONENTOUTPUTITEMINTERFACE_H

#include <QObject>
#include <QString>

/**
 * @brief This class defines the items constituing an output of a component:
 *      - the key identifier (must be UNIQUE)
 *      - the type: Boolean or Integer or Float or Double or String
 *      - the title (modifiable)
 *      - the unit (should be SI unit)
 */
class ComponentOutputItemInterface : public QObject
{
    Q_OBJECT

public:
    using ID = unsigned int;
    using Type = QString;
    using Title = QString;
    using Unit = QString;

public:
    explicit ComponentOutputItemInterface(QObject *const parent = nullptr) :
        QObject(parent)
    {
    }
    virtual ~ComponentOutputItemInterface() = default;

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that a component output has been successfully
    //! removed (used to update the table of outputs in the Window View of the
    //! Component Generator when users pressing on Delete button).
    //-----------------------------------------------------------------------------
    void cleared() const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that the type of a component output has been
    //! modified.
    //-----------------------------------------------------------------------------
    void modifiedType() const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that the title of a component output has been
    //! modified.
    //-----------------------------------------------------------------------------
    void modifiedTitle() const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that the unit of a component output has been
    //! modified.
    //-----------------------------------------------------------------------------
    void modifiedUnit() const;

public:
    //-----------------------------------------------------------------------------
    //! Resets the items constituing the output of the component.
    //! (e.g. function called when users pressing on Delete button of the table of
    //! outputs in the Window View of the Component Generator).
    //!
    //! @return                     True if output items successfully cleared.
    //-----------------------------------------------------------------------------
    virtual bool clear() = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets a key identifier \a id for the current registered output.
    //!
    //! @param[in]      id          The key identifier of the output.
    //! @return                     True if the output id successfully modified.
    //-----------------------------------------------------------------------------
    virtual bool setID(ComponentOutputItemInterface::ID const &id) = 0;

    //-----------------------------------------------------------------------------
    //! Gets the key identifier of the current registered output.
    //!
    //! @return                     The key identifier of the output.
    //-----------------------------------------------------------------------------
    virtual ComponentOutputItemInterface::ID getID() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets the type of the current registered output.
    //!
    //! @param[in]      type        The type of the output.
    //! @return                     True if the output type successfully modified.
    //-----------------------------------------------------------------------------
    virtual bool setType(ComponentOutputItemInterface::Type const &type) = 0;

    //-----------------------------------------------------------------------------
    //! Gets the type of the current registered output.
    //!
    //! @return                     The type of the output.
    //-----------------------------------------------------------------------------
    virtual ComponentOutputItemInterface::Type getType() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets the title of the current registered output.
    //!
    //! @param[in]      title       The title of the output.
    //! @return                     True if the output title successfully modified.
    //-----------------------------------------------------------------------------
    virtual bool setTitle(ComponentOutputItemInterface::Title const &title) = 0;

    //-----------------------------------------------------------------------------
    //! Gets the title of the current registered output.
    //!
    //! @return                     The title of the output.
    //-----------------------------------------------------------------------------
    virtual ComponentOutputItemInterface::Title getTitle() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets the unit of the current registered output.
    //!
    //! @param[in]      unit        The unit of the output.
    //! @return                     True if the output unit successfully modified.
    //-----------------------------------------------------------------------------
    virtual bool setUnit(ComponentOutputItemInterface::Unit const &unit) = 0;

    //-----------------------------------------------------------------------------
    //! Gets the unit of the current registered output.
    //!
    //! @return                     The unit of the output.
    //-----------------------------------------------------------------------------
    virtual ComponentOutputItemInterface::Unit getUnit() const = 0;
};

#endif // COMPONENTOUTPUTITEMINTERFACE_H
