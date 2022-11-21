/********************************************************************************
 * Copyright (c) 2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
/** \file  eventInterface.h
 *	\brief This file provides the interface for the events
 */
//-----------------------------------------------------------------------------
#pragma once

#include <vector>

#include "common/openPassTypes.h"
#include "common/eventTypes.h"

struct EventParameter
{
    std::string key;
    std::string value;
};

using EventParameters = std::vector<EventParameter>;

//-----------------------------------------------------------------------------
/** \brief This interface provides access to common event operations
*
* \ingroup Event
*/
//-----------------------------------------------------------------------------
class EventInterface
{
public:
    EventInterface() = default;
    EventInterface(const openpass::type::TriggeringEntities triggeringAgents, const openpass::type::AffectedEntities actingAgents) :
        triggeringAgents{std::move(triggeringAgents)},
        actingAgents{std::move(actingAgents)}{}

    EventInterface(const EventInterface &) = default;
    EventInterface(EventInterface &&) = default;
    EventInterface &operator=(const EventInterface & rhs) = default;
    EventInterface &operator=(EventInterface &&) = default;
    virtual ~EventInterface() = default;

    /*!
    * \brief Returns the time of the event.
    * \return	     Time in milliseconds.
    */
    virtual int GetEventTime() const = 0;

    /*!
    * \brief Returns the category of the event.
    * \return	     EventCategory.
    */
    virtual EventDefinitions::EventCategory GetCategory() const = 0;

    /*!
    * \brief Returns the name of the event.
    * \return	     Name of the event as string.
    */
    virtual std::string GetName() const = 0;

    /*!
    * \brief Returns the name of the source component.
    * \return	     Name of source component as string.
    */
    virtual std::string GetSource() const = 0;

    /*!
     * \brief GetTriggeringAgents
     * \return  List of triggering agents (might me empty)
     */
    virtual const openpass::type::TriggeringEntities GetTriggeringAgents() const = 0;

    /*!
     * \brief GetActingAgents
     * \return  List of acting agents (might me empty)
     */
    virtual const openpass::type::AffectedEntities GetActingAgents() const = 0;

    /*!
     * \brief Returns all parameter in their raw form
     * \see openpass::events::Parameter
     * \return Parameter
     */
    virtual const openpass::type::FlatParameter &GetParameter() const = 0;

public:
    //! All agents which triggered the event
    openpass::type::TriggeringEntities triggeringAgents {};

    //! All agents which are affected by the event
    openpass::type::AffectedEntities actingAgents {};
};
