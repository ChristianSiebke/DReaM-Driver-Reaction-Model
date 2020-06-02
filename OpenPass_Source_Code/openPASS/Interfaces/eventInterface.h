/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
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

#include<vector>

#include "Common/eventTypes.h"

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
    EventInterface(const EventInterface&) = delete;
    EventInterface(EventInterface&&) = delete;
    EventInterface& operator=(const EventInterface&) = delete;
    EventInterface& operator=(EventInterface&&) = delete;
    virtual ~EventInterface() = default;

    virtual void SetId(const int eventId) = 0;

    virtual int GetId() const = 0;

    virtual int GetEventTime() const = 0;

    virtual EventDefinitions::EventCategory GetCategory() const  = 0;

    virtual void SetTriggeringEventId(const int triggeringEventId) = 0;

    virtual int GetTriggeringEventId() const = 0;

    virtual std::string GetName() const = 0;

    virtual std::string GetSource() const = 0;

    virtual EventParameters GetParametersAsString() = 0;
};

