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
/** \addtogroup Event
* @{
* \file  BasicEvent.h
* \brief This file contains all basic functionality of events.
*
* \details This file contains the common event member variables and the basic functions of all events.
* @} */
//-----------------------------------------------------------------------------

#pragma once

#include <vector>

#include "include/eventInterface.h"

namespace openpass::events {

//-----------------------------------------------------------------------------
/** Signal This class contains all functionality of the BasicEvent.
 *
 * \ingroup Event */
//-----------------------------------------------------------------------------
template <EventDefinitions::EventCategory EventCategory>
class BasicEvent : public EventInterface
{
public:
    BasicEvent(int time, std::string eventName, std::string source,
               const TriggeringEntities triggeringAgents, const AffectedEntities actingAgents) :
        EventInterface{std::move(triggeringAgents), std::move(actingAgents)},
        time(time),
        name(std::move(eventName)),
        source(std::move(source))
    {
    }

    BasicEvent(int time, std::string eventName, std::string source) :
        EventInterface{}, time(time), name(std::move(eventName)), source(std::move(source))
    {
    }

    virtual int GetEventTime() const override
    {
        return time;
    }

    EventDefinitions::EventCategory GetCategory() const override
    {
        return EventCategory;
    }

    virtual std::string GetName() const override
    {
        return name;
    }

    virtual std::string GetSource() const override
    {
        return source;
    }

    virtual const TriggeringEntities GetTriggeringAgents() const override
    {
        return triggeringAgents;
    }

    virtual const AffectedEntities GetActingAgents() const override
    {
        return actingAgents;
    }

    const openpass::type::FlatParameter &GetParameter() const noexcept override
    {
        return parameter;
    }

private:
    const int time;
    const std::string name;
    const std::string source;

protected:
    openpass::type::FlatParameter parameter{};
};

using OpenPassEvent = BasicEvent<EventDefinitions::EventCategory::OpenPASS>;
using OpenScenarioEvent = BasicEvent<EventDefinitions::EventCategory::OpenSCENARIO>;

} // namespace openpass::events
