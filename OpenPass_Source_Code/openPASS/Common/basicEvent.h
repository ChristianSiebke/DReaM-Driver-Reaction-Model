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

#include "Interfaces/eventInterface.h"

//-----------------------------------------------------------------------------
/** Signal This class contains all functionality of the BasicEvent.
 *
 * \ingroup Event */
//-----------------------------------------------------------------------------
class BasicEvent : public EventInterface
{
public:
    BasicEvent(int time, std::string eventName, std::string source,
               const std::vector<int> triggeringAgents, const std::vector<int> actingAgents) :
        time(time),
        name(eventName),
        source(source),
        triggeringAgents(triggeringAgents),
        actingAgents(actingAgents)
    {
    }

    BasicEvent(int time, std::string eventName, std::string source) :
        time(time),
        name(eventName),
        source(source),
        triggeringAgents{},
        actingAgents{}
    {
    }

    virtual ~BasicEvent() override = default;

    virtual void SetId(const int eventId) override
    {
        this->eventId = eventId;
    }

    virtual int GetId() const override
    {
        return eventId;
    }

    virtual int GetEventTime() const override
    {
        return time;
    }

    virtual EventDefinitions::EventCategory GetCategory() const override
    {
        return EventDefinitions::EventCategory::Basic;
    }

    virtual void SetTriggeringEventId(const int triggeringEventId) override
    {
        this->triggeringEventId = triggeringEventId;
    }

    virtual int GetTriggeringEventId() const override
    {
        return triggeringEventId;
    }

    virtual std::string GetName() const override
    {
        return name;
    }

    virtual std::string GetSource() const override
    {
        return source;
    }

    virtual const std::vector<int> GetTriggeringAgents() const override
    {
        return triggeringAgents;
    }

    virtual const std::vector<int> GetActingAgents() const override
    {
        return actingAgents;
    }

    virtual EventParameters GetParametersAsString() override
    {
        EventParameters eventParameters;

        std::string triggeringAgentsAsString{};
        for (auto &triggeringAgentId : triggeringAgents)
        {
            triggeringAgentsAsString += std::to_string(triggeringAgentId) + ",";
        }

        std::string actingAgentsAsString;
        for (auto &actingAgentId : actingAgents)
        {
            actingAgentsAsString += std::to_string(actingAgentId) + ",";
        }

        if (!triggeringAgentsAsString.empty())
        {
            triggeringAgentsAsString.pop_back();
            eventParameters.push_back({"TriggeringAgentIds", triggeringAgentsAsString});
        }

        if (!actingAgentsAsString.empty())
        {
            actingAgentsAsString.pop_back();
            eventParameters.push_back({"ActingAgentIds", actingAgentsAsString});
        }

        return eventParameters;
    }

    const openpass::type::FlatParameter &GetParameter() const noexcept override
    {
        return parameter;
    }

private:
    const int time;
    const std::string name;
    const std::string source;

    int eventId{};
    int triggeringEventId{-1};

public:
    const std::vector<int> triggeringAgents;
    const std::vector<int> actingAgents;

protected:
    openpass::type::FlatParameter parameter{};
};
