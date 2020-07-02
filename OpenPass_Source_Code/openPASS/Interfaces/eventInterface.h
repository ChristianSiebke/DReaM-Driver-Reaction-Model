/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \file  eventInterface.h
 *	\brief This file provides the interface for the events
 */
//-----------------------------------------------------------------------------
#pragma once

#include <vector>

#include "Common/eventTypes.h"
#include "Common/openPassTypes.h"

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
    EventInterface(const EventInterface &) = delete;
    EventInterface(EventInterface &&) = delete;
    EventInterface &operator=(const EventInterface &) = delete;
    EventInterface &operator=(EventInterface &&) = delete;
    virtual ~EventInterface() = default;

    /*!
    * \brief Sets the Id of the event.
    * \param[in]	Id of the event.
    */
    virtual void SetId(const int eventId) = 0;

    /*!
    * \brief Returns the Id of the event.
    * \return	     Id.
    */
    virtual int GetId() const = 0;

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
    * \brief Sets the Id of the event which triggered this event.
    * \param[in]	     Event Id.
    */
    virtual void SetTriggeringEventId(const int triggeringEventId) = 0;

    /*!
    * \brief Returns the Id of the event which triggered this event.
    * \return	     Event Id.
    */
    virtual int GetTriggeringEventId() const = 0;

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
    virtual const std::vector<int> GetTriggeringAgents() const = 0;

    /*!
     * \brief GetActingAgents
     * \return  List of acting agents (might me empty)
     */
    virtual const std::vector<int> GetActingAgents() const = 0;

    /*!
    * \brief Returns all parameters of the event as string list.
    * \details Returns the agentId as string list.
    * \return	     List of string pairs of the event parameters.
    */
    [[deprecated("Will be replaced by functionality of openpass::events::Parameter")]] virtual EventParameters GetParametersAsString() = 0;

    /*!
     * \brief Returns all parameter in their raw form
     * \see openpass::events::Parameter
     * \return Parameter
     */
    [[deprecated("will be replaced by specializations of the type")]] virtual const openpass::type::FlatParameter &GetParameter() const = 0;
};
