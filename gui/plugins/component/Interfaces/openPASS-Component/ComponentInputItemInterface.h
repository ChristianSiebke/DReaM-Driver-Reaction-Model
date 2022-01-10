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
//! @file  ComponentInputItemInterface.h
//! @ingroup componentPlugin
//! @brief The Component Input Item Interface (CIII) contains the functions
//! setters and getters of the items constituing an input of a component.
//! This interface belongs to the Component Plugin (CP).
//!
//-----------------------------------------------------------------------------

#ifndef COMPONENTINPUTITEMINTERFACE_H
#define COMPONENTINPUTITEMINTERFACE_H

#include <QObject>
#include <QString>

/**
 * @brief This class defines the items constituing an input of a component:
 *      - the key identifier (must be UNIQUE)
 *      - the type: Boolean or Integer or Float or Double or String
 *      - the title (modifiable)
 *      - the unit (should be SI unit)
 *      - the cardinality (number of inputs accepted)
 */
class ComponentInputItemInterface : public QObject
{
    Q_OBJECT

public:
    using ID = unsigned int;
    using Type = QString;
    using Title = QString;
    using Unit = QString;
    using Cardinality = QString;

public:
    explicit ComponentInputItemInterface(QObject *const parent = nullptr) :
        QObject(parent)
    {
    }
    virtual ~ComponentInputItemInterface() = default;

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that a component input has been successfully
    //! removed (used to update the table of inputs in the Window View of the
    //! Component Generator when users pressing on Delete button).
    //-----------------------------------------------------------------------------
    void cleared() const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that the type of a component input has been
    //! modified.
    //-----------------------------------------------------------------------------
    void modifiedType() const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that the title of a component input has been
    //! modified.
    //-----------------------------------------------------------------------------
    void modifiedTitle() const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that the unit of a component input has been
    //! modified.
    //-----------------------------------------------------------------------------
    void modifiedUnit() const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that the cadinality of a component input has been
    //! modified.
    //-----------------------------------------------------------------------------
    void modifiedCardinality() const;

public:
    //-----------------------------------------------------------------------------
    //! Resets the items constituing the input of the component.
    //! (e.g. function called when users pressing on Delete button of the table of
    //! inputs in the Window View of the Component Generator).
    //!
    //! @return                         True if input items successfully cleared.
    //-----------------------------------------------------------------------------
    virtual bool clear() = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets a key identifier \a id for the current registered input.
    //!
    //! @param[in]      id              The input key identifier.
    //! @return                         True if the input id successfully modified.
    //-----------------------------------------------------------------------------
    virtual bool setID(ComponentInputItemInterface::ID const &id) = 0;

    //-----------------------------------------------------------------------------
    //! Gets the key identifier of the current registered input.
    //!
    //! @return                         The key identifier of the input.
    //-----------------------------------------------------------------------------
    virtual ComponentInputItemInterface::ID getID() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets the type of the current registered input.
    //!
    //! @param[in]      type            The type of the input.
    //! @return                         True if the input type successfully modified.
    //-----------------------------------------------------------------------------
    virtual bool setType(ComponentInputItemInterface::Type const &type) = 0;

    //-----------------------------------------------------------------------------
    //! Gets the type of the current registered input.
    //!
    //! @return                         The type of the input.
    //-----------------------------------------------------------------------------
    virtual ComponentInputItemInterface::Type getType() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets the title of the current registered input.
    //!
    //! @param[in]      title           The title of the input.
    //! @return                         True if the input title successfully modified.
    //-----------------------------------------------------------------------------
    virtual bool setTitle(ComponentInputItemInterface::Title const &title) = 0;

    //-----------------------------------------------------------------------------
    //! Gets the title of the current registered input.
    //!
    //! @return                         The title of the input.
    //-----------------------------------------------------------------------------
    virtual ComponentInputItemInterface::Title getTitle() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets the unit of the current registered input.
    //!
    //! @param[in]      unit            The unit of the input.
    //! @return                         True if the input unit successfully modified.
    //-----------------------------------------------------------------------------
    virtual bool setUnit(ComponentInputItemInterface::Unit const &unit) = 0;

    //-----------------------------------------------------------------------------
    //! Gets the unit of the current registered input.
    //!
    //! @return                         The unit of the input.
    //-----------------------------------------------------------------------------
    virtual ComponentInputItemInterface::Unit getUnit() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets the cardinality of the current registered input.
    //!
    //! @param[in]      cardinality     The cardinality for the input.
    //! @return                         True if the cardinality successfully modified.
    //-----------------------------------------------------------------------------
    virtual bool setCardinality(ComponentInputItemInterface::Cardinality const &cardinality) = 0;

    //-----------------------------------------------------------------------------
    //! Gets the cardinality of the current registered input.
    //!
    //! @return                         The cardinality for the input.
    //-----------------------------------------------------------------------------
    virtual ComponentInputItemInterface::Cardinality getCardinality() const = 0;
};

#endif // COMPONENTINPUTITEMINTERFACE_H
