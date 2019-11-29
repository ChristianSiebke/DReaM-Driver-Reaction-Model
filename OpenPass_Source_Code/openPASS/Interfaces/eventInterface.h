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

#include<list>

#include "Common/eventTypes.h"

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

    virtual void SetEventId(const int eventId) = 0;

    virtual int GetId() const = 0;

    virtual int GetEventTime() const = 0;

    virtual void SetTriggeringEventId(const int triggeringEventId) = 0;

    virtual int GetTriggeringEventId() const = 0;

    virtual std::string GetSource() const = 0;

    virtual std::string GetSequenceName() const = 0;

    virtual EventDefinitions::EventType GetEventType() const = 0;

    virtual std::list<std::pair<std::string, std::string>> GetEventParametersAsString() = 0;
};

