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
//! @file  ComponentScheduleInterface.h
//! @ingroup componentPlugin
//! @brief The Component Schedule Interface (CSI) contains the functions setters
//! and getters for the schedule parameters of a component.
//! This interface belongs to the Component Plugin (CP).
//!
//-----------------------------------------------------------------------------

#ifndef COMPONENTSCHEDULEINTERFACE_H
#define COMPONENTSCHEDULEINTERFACE_H

#include <QObject>

/**
 * @brief This class defines the schedule parameters of a component:
 *      - Offset Time
 *      - Cycle Time
 *      - Response Time
 */
class ComponentScheduleInterface : public QObject
{
    Q_OBJECT

public:
    using Offset = unsigned int;
    using Cycle = unsigned int;
    using Response = unsigned int;

public:
    explicit ComponentScheduleInterface(QObject * const parent = nullptr)
        : QObject(parent) {}
    virtual ~ComponentScheduleInterface() = default;

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that the schedule parameters of the current
    //! component has been reset successfully.
    //-----------------------------------------------------------------------------
    void cleared() const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that the Offset Time of a component has been
    //! modified.
    //-----------------------------------------------------------------------------
    void modifiedOffset() const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that the Cycle Time of a component has been
    //! modified.
    //-----------------------------------------------------------------------------
    void modifiedCycle() const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that the Response Time of a component has been
    //! modified.
    //-----------------------------------------------------------------------------
    void modifiedResponse() const;

public:
    //-----------------------------------------------------------------------------
    //! Clears the items constituing the schedule parameters of the current component.
    //!
    //! @return                     True if schedule items successfully cleared.
    //-----------------------------------------------------------------------------
    virtual bool clear() = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets the Offset Time for the current registered component.
    //!
    //! @param[in]      offset      The Offset Time for the component.
    //! @return                     True if the Offset Time successfully modified.
    //-----------------------------------------------------------------------------
    virtual bool setOffset(ComponentScheduleInterface::Offset const & offset) = 0;

    //-----------------------------------------------------------------------------
    //! Gets the Offset Time for the current registered component.
    //!
    //! @return                     The Offset Time.
    //-----------------------------------------------------------------------------
    virtual ComponentScheduleInterface::Offset getOffset() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets the Cycle Time for the current registered component.
    //!
    //! @param[in]      cycle       The Cycle Time for the component.
    //! @return                     True if the Cycle Time successfully modified.
    //-----------------------------------------------------------------------------
    virtual bool setCycle(ComponentScheduleInterface::Cycle const & cycle) = 0;

    //-----------------------------------------------------------------------------
    //! Gets the Cycle Time for the current registered component.
    //!
    //! @return                     The Cycle Time.
    //-----------------------------------------------------------------------------
    virtual ComponentScheduleInterface::Cycle getCycle() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets the Response Time for the current registered component.
    //!
    //! @param[in]      response    The Response Time for the component.
    //! @return                     True if the Response Time successfully modified.
    //-----------------------------------------------------------------------------
    virtual bool setResponse(ComponentScheduleInterface::Response const & response) = 0;

    //-----------------------------------------------------------------------------
    //! Gets the Response Time for the current registered component.
    //!
    //! @return                     The Response Time.
    //-----------------------------------------------------------------------------
    virtual ComponentScheduleInterface::Response getResponse() const = 0;
};

#endif // COMPONENTSCHEDULEINTERFACE_H
