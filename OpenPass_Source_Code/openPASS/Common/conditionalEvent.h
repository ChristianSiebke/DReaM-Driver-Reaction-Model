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
/** @file  AgentBasedEvent.h
* @brief This file contains all functions for agent based events.
*
* This class contains all functionality of the module. */
//-----------------------------------------------------------------------------

#pragma once

#include <vector>

#include "basicEvent.h"

//-----------------------------------------------------------------------------
/** This class implements all functionality of the AgentBasedEvent.
 *
 * \ingroup Event */
//-----------------------------------------------------------------------------
class ConditionalEvent : public BasicEvent
{
public:
    ConditionalEvent(int time,
                     std::string eventName,
                    std::string source,
                    std::vector<int> triggeringAgents,
                    std::vector<int> actingAgents):
        BasicEvent(time,
                   eventName,
                   source),
        triggeringAgents(triggeringAgents),
        actingAgents(actingAgents)
    {}
    ~ConditionalEvent() override = default;

    /*!
    * \brief Returns the category of the event.
    *
    * @return	     EventCategory.
    */
    virtual EventDefinitions::EventCategory GetCategory() const override
    {
        return EventDefinitions::EventCategory::Conditional;
    }

    /*!
    * \brief Returns all parameters of the event as string list.
    * \details Returns the agentId as string list.
    *
    * @return	     List of string pairs of the event parameters.
    */
    EventParameters GetParametersAsString() override
    {
        EventParameters eventParameters;

        std::string triggeringAgentsAsString{};
        for (auto &triggeringAgentId : triggeringAgents) {
            triggeringAgentsAsString += std::to_string(triggeringAgentId) + ",";
        }

        std::string actingAgentsAsString;
        for (auto &actingAgentId : actingAgents) {
            actingAgentsAsString += std::to_string(actingAgentId) + ",";
        }

        if(triggeringAgentsAsString.size() > 0)
        {
            triggeringAgentsAsString.pop_back();
            eventParameters.push_back({"TriggeringAgentIds", triggeringAgentsAsString});
        }
        if(actingAgentsAsString.size() > 0)
        {
            actingAgentsAsString.pop_back();
            eventParameters.push_back({"ActingAgentIds", actingAgentsAsString});
        }

        return eventParameters;
    }

    const std::vector<int> triggeringAgents;
    const std::vector<int> actingAgents;
};

